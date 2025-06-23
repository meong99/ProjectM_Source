#include "MMonsterBase.h"
#include "Interaction/MInteractionComponent.h"
#include "AbilitySystem/PMAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/PMHealthSet.h"
#include "Character/Components/PMHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "Inventory/PMInventoryManagerComponent.h"
#include "Character/Components/MWalletComponent.h"
#include "AIController.h"
#include "Definitions/MMonsterDefinition.h"
#include "PMGameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "Components/MMonsterTradeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Util/MGameplayStatics.h"
#include "System/MDataTableManager.h"
#include "Table/MTable_MonsterTable.h"
#include "AbilitySystem/PMAbilitySet.h"
#include "AbilitySystem/Attributes/PMCombatSet.h"
#include "AbilitySystem/MGameplayEffectSet.h"
#include "Types/MTeamTypes.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffectExtension.h"
#include "Components/WidgetComponent.h"
#include "UI/Text/MDamageTextWidget.h"
#include "Perception/AISense_Damage.h"
#include "Character/MPlayerCharacterBase.h"
#include "AbilitySystemBlueprintLibrary.h"

AMMonsterBase::AMMonsterBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bRequestedMoveUseAcceleration = true;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	bUseControllerRotationYaw = true;

	GetCapsuleComponent()->SetCollisionProfileName(*UEnum::GetDisplayValueAsText(EMCollisionChannel::Monster).ToString());

	InteractionComponent = CreateDefaultSubobject<UMInteractionComponent>(TEXT("InteractionComponent"));
	InteractionComponent->SetupAttachment(GetRootComponent());

	AbilitySystemComponent = CreateDefaultSubobject<UPMAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AbilitySystemComponent->bWantsInitializeComponent = true;
	HealthSet = CreateDefaultSubobject<UPMHealthSet>(TEXT("HealthSet"));
	CombatSet = CreateDefaultSubobject<UPMCombatSet>(TEXT("CombatSet"));

	MonsterTradeComponent = CreateDefaultSubobject<UMMonsterTradeComponent>(TEXT("MonsterTradeComponent"));
	DamageWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamageWidget"));
	DamageWidgetComponent->SetupAttachment(RootComponent);

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
}

void AMMonsterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AMMonsterBase::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	UMDataTableManager* TableManager = GEngine->GetEngineSubsystem<UMDataTableManager>();

	if (TableManager)
	{
		const UDataTable* MonsterTable = TableManager->GetDataTable(MonsterRowId);
		if (MonsterTable)
		{
			int32 ElementIndex = UMDataTableManager::ChangeRowIdToElementId(MonsterRowId) - 1;
			const TArray<FName>& Names = MonsterTable->GetRowNames();
			if (Names.IsValidIndex(ElementIndex))
			{
				FMTable_MonsterTable* RowData = MonsterTable->FindRow<FMTable_MonsterTable>(Names[ElementIndex], Names[ElementIndex].ToString());
				if (RowData && RowData->Definition)
				{
					MonsterDefinition = DuplicateObject(RowData->Definition->GetDefaultObject<UMMonsterDefinition>(), this);
					GetCharacterMovement()->MaxWalkSpeed = MonsterDefinition->GetMonsterInfo().MonsterMaxSpeed;
				}
			}
		}
	}

	if (GetNetMode() != ENetMode::NM_DedicatedServer && IsValid(MonsterDefinition))
	{
		InteractionComponent->SetNewInteractions(MonsterDefinition->Action_OnBeginOverlap, MonsterDefinition->Action_OnInteract);
	}
}

void AMMonsterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (HasAuthority())
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		Server_SetCharacterLifeState(EMCharacterLiftState::Spawned);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UPMHealthSet::GetHealthAttribute()).AddUObject(this, &AMMonsterBase::Callback_OnDamaged);

		if (MonsterDefinition)
		{
			UMGameplayEffectSet* EffectSet = MonsterDefinition->MonsterInfo.EffectSet;
			if (EffectSet && AbilitySystemComponent)
			{
				EffectSet->ApplyGameplayEffectsToAsc(AbilitySystemComponent, &AppliedDefaultEffects, AbilitySystemComponent->GetOwner(), nullptr, this);
			}

			FMAbilitySet_GrantedHandles TempGrantedHandles;
			for (const UPMAbilitySet* AbilitySet : MonsterDefinition->AbilitySets)
			{
				AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, &TempGrantedHandles, this, MonsterDefinition->RowId);
			}

			GrantedHandles.Add(MonsterDefinition->RowId, TempGrantedHandles);
		}
	}
}

void AMMonsterBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		Server_SetCharacterLifeState(EMCharacterLiftState::Alive);
	}

	SetGenericTeamId((int32)EMGenericTeamId::Monster);
}

void AMMonsterBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (HasAuthority())
	{
		if (HealthSet->GetHealth() <= 0.0f && CharacterLifeState == EMCharacterLiftState::Alive)
		{
			Server_SetCharacterLifeState(EMCharacterLiftState::ReadyToDead);
			OnDead();
		}

		FVector Location = GetActorLocation();
		if (Location.Z < -10000.f)
		{
			Destroy();
		}
	}
}

void AMMonsterBase::InitCharacterName()
{
	if (MonsterDefinition)
	{
		CharacterName = MonsterDefinition->GetMonsterName();
	}
	Super::InitCharacterName();
}

void AMMonsterBase::OnDead()
{
	Super::OnDead();

	GiveRewardToPlayer();

	if (HasAuthority())
	{
		Server_SetCharacterLifeState(EMCharacterLiftState::Dead);
		Server_SetCharacterLifeState(EMCharacterLiftState::ReadyToDestroy);

		if (MonsterDefinition->DeathAnimation.Montage)
		{
			float Duration = MonsterDefinition->DeathAnimation.Montage->GetPlayLength();
			Multicast_PlayMontage(MonsterDefinition->DeathAnimation.Montage);
			UMGameplayStatics::SetTimer(this, [this]()->void
			{
				AAIController* AIController = Cast<AAIController>(GetController());
				if (AIController)
				{
					AIController->UnPossess();
				}

				Destroy();
			}, Duration, false);
		}
	}
}

void AMMonsterBase::Destroyed()
{
	Super::Destroyed();

	if (HasAuthority())
	{
		RequestRemoveToSpawner();
	}
}

void AMMonsterBase::SetSpawner(AMMonsterSpawner* InSpawner)
{
	MonsterSpawner = InSpawner;
}

UPMAbilitySystemComponent* AMMonsterBase::GetMAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMMonsterBase::Callback_OnDamaged(const FOnAttributeChangeData& ChangeData)
{
	if (ChangeData.GEModData != nullptr)
	{
		const FGameplayEffectContextHandle& EffectContextHandle = ChangeData.GEModData->EffectSpec.GetEffectContext();
		const FGameplayEffectContext* Context = EffectContextHandle.Get();
		UAbilitySystemComponent* PlayerAsc = Context ? Context->GetInstigatorAbilitySystemComponent() : nullptr;
		AMPlayerCharacterBase* PlayerCharacter = PlayerAsc ? Cast<AMPlayerCharacterBase>(PlayerAsc->GetAvatarActor()) : nullptr;
		if (PlayerCharacter)
		{
			LastAttacker = EffectContextHandle.GetOriginalInstigator();
			UAISense_Damage::ReportDamageEvent(this, this, PlayerCharacter, ChangeData.GEModData->EvaluatedData.Magnitude, GetActorLocation(), GetActorLocation());
		}
	}
}

void AMMonsterBase::GiveRewardToPlayer()
{
	APlayerState* AttackerPlayerState = Cast<APlayerState>(LastAttacker);
	if (AttackerPlayerState && MonsterDefinition)
	{
		APlayerController* AttackerController = AttackerPlayerState->GetPlayerController();
		UMTradeComponentBase* PlayerTradeComponent = AttackerController ? AttackerController->FindComponentByClass<UMTradeComponentBase>() : nullptr;
		if (PlayerTradeComponent)
		{
			FMTradeRequest Request = MonsterDefinition->CreateTradeRequest();
			PlayerTradeComponent->Server_OnRequestSimpleDataGrant(this, Request);
		}

		FGameplayEventData EventData;
		EventData.Target = LastAttacker;
		EventData.EventMagnitude = MonsterDefinition->MonsterInfo.ExperiencePoint;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(LastAttacker, FPMGameplayTags::Get().GameplayEvent_Character_ChangeExp, EventData);
	}
}

void AMMonsterBase::RequestRemoveToSpawner()
{
	if (MonsterSpawner)
	{
		MonsterSpawner->OnDeadMonster(this);
		MonsterSpawner = nullptr;
	}
}

UAbilitySystemComponent* AMMonsterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
