#include "MNpcBase.h"
#include "Interaction/MInteractionComponent.h"
#include "MNpcDefinition.h"
#include "Components/CapsuleComponent.h"
#include "Util/MGameplayStatics.h"
#include "Engine/Engine.h"
#include "System/MDataTableManager.h"
#include "Table/Item/MTable_NPC.h"
#include "GameModes/PMGameStateBase.h"

AMNpcBase::AMNpcBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	InteractionComponent = CreateDefaultSubobject<UMInteractionComponent>(TEXT("InteractionComponent"));
	InteractionComponent->SetupAttachment(GetRootComponent());
	InteractionComponent->SetSphereRadius(300.f);

	GetCapsuleComponent()->SetCollisionProfileName(*UEnum::GetDisplayValueAsText(EMCollisionChannel::NPC).ToString());
	bReplicates = true;
}

void AMNpcBase::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UMDataTableManager* TableManager = GEngine->GetEngineSubsystem<UMDataTableManager>();

	if (TableManager)
	{
		const UDataTable* NpcTable = TableManager->GetDataTable(NpcRowId);
		if (NpcTable)
		{
			int32 ElementIndex = UMDataTableManager::ChangeRowIdToElementId(NpcRowId) - 1;
			const TArray<FName>& Names = NpcTable->GetRowNames();
			if (Names.IsValidIndex(ElementIndex))
			{
				FMTable_NPC* RowData = NpcTable->FindRow<FMTable_NPC>(Names[ElementIndex], Names[ElementIndex].ToString());
				if (RowData && RowData->Definition)
				{
					NpcDefinition = DuplicateObject(RowData->Definition->GetDefaultObject<UMNpcDefinition>(), this);
				}
			}
		}
	}

	if (GetNetMode() != ENetMode::NM_DedicatedServer && IsValid(NpcDefinition))
	{
		InteractionComponent->SetNewInteractions(NpcDefinition->Action_OnBeginOverlap, NpcDefinition->Action_OnInteract);
	}
}

void AMNpcBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AMNpcBase::BeginPlay()
{
	Super::BeginPlay();

	APMGameStateBase* GameState = Cast<APMGameStateBase>(GetWorld()->GetGameState());
	if (GameState && NpcDefinition && NpcDefinition->SearchTag.IsValid())
	{
		GameState->TagMappedActor.Add(NpcDefinition->SearchTag, this);
	}
}

void AMNpcBase::InitCharacterName()
{
	if (NpcDefinition)
	{
		CharacterName = NpcDefinition->NpcName;
	}
	Super::InitCharacterName();
}
