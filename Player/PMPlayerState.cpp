#include "PMPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "GameModes/PMExperienceManagerComponent.h"
#include "GameModes/PMGameModeBase.h"
#include "GameModes/AsyncAction_ExperienceReady.h"
#include "Character/PMPawnData.h"
#include "AbilitySystem/PMAbilitySystemComponent.h"
#include "AbilitySystem/PMAbilitySet.h"
#include "AbilitySystem/Attributes/PMHealthSet.h"
#include "AbilitySystem/Attributes/PMCombatSet.h"
#include "Inventory/PMInventoryManagerComponent.h"
#include "Inventory/PMInventoryItemInstance.h"
#include "Equipment/PMQuickBarComponent.h"
#include "Equipment/PMEquipmentManagerComponent.h"
#include "System/MDataTableManager.h"
#include "PMGameplayTags.h"
#include "Player/MPlayerSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "Engine/Engine.h"
#include "Components/MPlayerStateComponentBase.h"
#include "Components/MLevelComponent.h"

#define PLAYER_ID 1
#define PLAYER_NAME TEXT("PlayerName")

APMPlayerState::APMPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UPMAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	EquipmentManager = CreateDefaultSubobject<UPMEquipmentManagerComponent>(TEXT("EquipmentManager"));
	LevelComponent = CreateDefaultSubobject<UMLevelComponent>(TEXT("LevelComponent"));
	CreateDefaultSubobject<UPMHealthSet>(TEXT("HealthSet"));
	CreateDefaultSubobject<UPMCombatSet>(TEXT("CombatSet"));

	NetUpdateFrequency = 100.0f;
	bReplicates = true;
}

void APMPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);

	UWorld* World = GetWorld();
	if (World && World->IsGameWorld() && World->GetNetMode() != NM_Client)
	{
		const AGameStateBase* GameState = World->GetGameState();
		check(GameState);

		UPMExperienceManagerComponent* ExperienceManagerComp = GameState->FindComponentByClass<UPMExperienceManagerComponent>();
		check(ExperienceManagerComp);

		ExperienceManagerComp->CallOrRegister_OnExperienceLoaded(FOnExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}

	OnPawnSet.AddDynamic(this, &APMPlayerState::OnSetNewPawn);
}

void APMPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void APMPlayerState::OnExperienceLoaded(const UPMExperienceDefinition* CurrentExperience)
{
	if (APMGameModeBase* GameMode = GetWorld()->GetAuthGameMode<APMGameModeBase>())
	{
		// PlayerState에 있는 PawnData를 사용해도 되지만 없을수도 있고, 기본적으로 PawnData를 가져오는 곳을 하나로 통일해서 관리를 수월하게 하기 위해 GameMode에서 관리한다.
		const UPMPawnData* NewPawnData = GameMode->GetPawnDataForController(GetOwningController());
		if (NewPawnData)
		{
			SetPawnData(NewPawnData);
			UAsyncAction_ExperienceReady* ExperienceReady = UAsyncAction_ExperienceReady::WaitForExperienceReady(this);
			if (ExperienceReady)
			{
				ExperienceReady->OnReady.AddDynamic(this, &ThisClass::Server_LoadPlayerData);
				ExperienceReady->Activate();
			}
		}
	}

	const TSet<UActorComponent*>& SubComponents = GetComponents();
	for (UActorComponent* Component : SubComponents)
	{
		UMPlayerStateComponentBase* SubComponent = Cast<UMPlayerStateComponentBase>(Component);
		if (SubComponent)
		{
			SubComponent->OnExperienceLoaded(CurrentExperience);
		}
	}
}

void APMPlayerState::SetPawnData(const UPMPawnData* InPawnData)
{
	check(InPawnData);

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	check(!PawnData);

	PawnData = InPawnData;

	for (UPMAbilitySet* AbilitySet : PawnData->GetAbilitySets())
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}
}

void APMPlayerState::Server_SavePlayerData_Implementation()
{
	UpdateCurrentData();
	UGameplayStatics::SaveGameToSlot(PlayerSaveData, PlayerSaveData->SaveSlotName, PlayerSaveData->SaveIndex);
}

void APMPlayerState::Server_LoadPlayerData_Implementation()
{
	if (!PlayerSaveData)
	{
		PlayerSaveData = Cast<UMPlayerSaveGame>(UGameplayStatics::LoadGameFromSlot(PLAYER_NAME, PLAYER_ID));
		if (PlayerSaveData)
		{
			ApplyLoadedData();
		}
	}
	else
	{
		ApplyLoadedData();
	}
}

void APMPlayerState::UpdateCurrentData()
{
	if (!PlayerSaveData)
	{
		PlayerSaveData = Cast<UMPlayerSaveGame>(UGameplayStatics::CreateSaveGameObject(UMPlayerSaveGame::StaticClass()));
	}

	PlayerSaveData->SaveIndex = PLAYER_ID;
	PlayerSaveData->SaveSlotName = PLAYER_NAME;
	APlayerController* Controller = GetPlayerController();
	UPMInventoryManagerComponent* InventoryManager = Controller ? Controller->FindComponentByClass<UPMInventoryManagerComponent>() : nullptr;

	if (InventoryManager)
	{
		PlayerSaveData->ItemDefinitions.Empty();

		const FPMInventoryItemList& EquipmentItemList = InventoryManager->GetEquipmentItemList();
		for (const FPMInventoryEntry& Entry : EquipmentItemList.Entries)
		{
			TSubclassOf<UPMInventoryItemDefinition> ItemDef = Entry.GetItemDefinition();
			if (ItemDef)
			{
				PlayerSaveData->ItemDefinitions.Add(ItemDef);
			}
		}

		const FPMInventoryItemList& ConsumableItemList = InventoryManager->GetConsumableItemList();
		for (const FPMInventoryEntry& Entry : ConsumableItemList.Entries)
		{
			TSubclassOf<UPMInventoryItemDefinition> ItemDef = Entry.GetItemDefinition();
			if (ItemDef)
			{
				PlayerSaveData->ItemDefinitions.Add(ItemDef);
			}
		}
	}
	else
	{
		MCHAE_WARNING("InventoryManager or QuickBarComp is null. Check the controller is created currectly");
	}
}

void APMPlayerState::ApplyLoadedData()
{
	APlayerController* Controller = GetPlayerController();
	if (Controller == nullptr)
	{
		return;
	}

	UPMInventoryManagerComponent* InventoryManager = Controller->FindComponentByClass<UPMInventoryManagerComponent>();
	UPMQuickBarComponent* QuickBarComp = Controller->FindComponentByClass<UPMQuickBarComponent>();

	if (InventoryManager && QuickBarComp)
	{
		for (const TSubclassOf<UPMInventoryItemDefinition>& ItemDef : PlayerSaveData->ItemDefinitions)
		{
			if (ItemDef)
			{
				FMItemRequest Request;
				Request.SetItemRequest(EMItemRequestType::InitItem, ItemDef->GetDefaultObject<UPMInventoryItemDefinition>()->RowId, 1);
				InventoryManager->RequestItemChange(Request);
			}
		}
	}
	else
	{
		MCHAE_WARNING("InventoryManager or QuickBarComp is null. Check the controller is created currectly");
	}
}

void APMPlayerState::OnSetNewPawn(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	const TSet<UActorComponent*>& SubComponents = GetComponents();
	for (UActorComponent* Component : SubComponents)
	{
		UMPlayerStateComponentBase* SubComponent = Cast<UMPlayerStateComponentBase>(Component);
		if (SubComponent)
		{
			SubComponent->OnSetNewPawn(NewPawn, OldPawn);
		}
	}
}

void APMPlayerState::Debug_SaveGame()
{
	Server_SavePlayerData();
}
