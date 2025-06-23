#include "MAIControllerBase.h"
#include "MMonsterBase.h"
#include "Definitions/MMonsterDefinition.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Types/MBlackboardTypes.h"
#include "Character/MCharacterBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Types/MTeamTypes.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AbilitySystem/PMAbilitySystemComponent.h"
#include "PMGameplayTags.h"
#include "GameplayEffectTypes.h"

AMAIControllerBase::AMAIControllerBase()
{
	PrimaryActorTick.bCanEverTick = true;

	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception Component"));

	DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));
	DamageConfig->SetMaxAge(5.0f);
	DamageConfig->SetStartsEnabled(true);

	PerceptionComponent->ConfigureSense(*DamageConfig);
	PerceptionComponent->SetDominantSense(DamageConfig->GetSenseImplementation());

	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AMAIControllerBase::OnTargetDetectedDelegated);
	PerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &AMAIControllerBase::OnTargetForgotDelegated);
}

void AMAIControllerBase::OnPossess(APawn* InPawn)
{
	AMCharacterBase* OldCharacter = Cast<AMCharacterBase>(GetPawn());
	if (OldCharacter && DelegateHandle_MonsterStateChange.IsValid())
	{
		OldCharacter->Delegate_OnChangeCharacterStateFlags.Remove(DelegateHandle_MonsterStateChange);
		DelegateHandle_MonsterStateChange.Reset();
	}

	Super::OnPossess(InPawn);

	if (AMMonsterBase* Monster = Cast<AMMonsterBase>(InPawn))
	{
		AbilitySystemComponent = Monster->GetMAbilitySystemComponent();
		DelegateHandle_MonsterStateChange = Monster->Delegate_OnChangeCharacterStateFlags.AddUObject(this, &AMAIControllerBase::OnChange_CharacterStateFlag);
		SetBehaviorTree(Monster);
	}
}

void AMAIControllerBase::OnUnPossess()
{
	Super::OnUnPossess();

	Destroy();
}

void AMAIControllerBase::OnChange_CharacterStateFlag(const int64& OldFlag, const int64& NewFlag)
{
	bool bCanMove = !(NewFlag & EMCharacterStateFlag::BlockMovement);

	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (BlackboardComp && UseBlackboard(BlackboardComp->GetBlackboardAsset(), BlackboardComp))
	{
		BlackboardComp->SetValueAsBool(BBKey::CAN_MOVE, bCanMove);
	}
}

void AMAIControllerBase::OnTargetDetectedDelegated(AActor* Actor, struct FAIStimulus Stimulus)
{
	IGenericTeamAgentInterface* TeamInterface = Cast<IGenericTeamAgentInterface>(Actor);
	if (!TeamInterface || DelegateHandle_TargetLiftStateChange.IsValid())
	{
		return;
	}

	if (TeamInterface->GetGenericTeamId() == FGenericTeamId::NoTeam)
	{
		return;
	}

	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (BlackboardComp && UseBlackboard(BlackboardComp->GetBlackboardAsset(), BlackboardComp))
	{
		BlackboardComp->SetValueAsObject(BBKey::TARGET_ACTOR, Actor);
		BlackboardComp->SetValueAsBool(BBKey::FOUND_PLAYER, true);
		BindTargetDeathCallback();
	}
}

void AMAIControllerBase::OnTargetForgotDelegated(AActor* Actor)
{
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (BlackboardComp && UseBlackboard(BlackboardComp->GetBlackboardAsset(), BlackboardComp))
	{
		BlackboardComp->SetValueAsObject(BBKey::TARGET_ACTOR, nullptr);
		BlackboardComp->SetValueAsBool(BBKey::FOUND_PLAYER, false);
		RemoveTargetDeathCallback();
	}
}

ETeamAttitude::Type AMAIControllerBase::GetTeamAttitudeTowards(const AActor& Other) const
{
	const IGenericTeamAgentInterface* OtherTeamAgent = Cast<const IGenericTeamAgentInterface>(&Other);
	const IGenericTeamAgentInterface* MyTeamAgent = Cast<const IGenericTeamAgentInterface>(GetPawn());
	
	if (OtherTeamAgent && MyTeamAgent)
	{
		FGenericTeamId OtherTeamID = OtherTeamAgent->GetGenericTeamId();
		FGenericTeamId MyTeamID = MyTeamAgent->GetGenericTeamId();
		if (OtherTeamID == MyTeamID)
		{
			return ETeamAttitude::Friendly;
		}
		else if (OtherTeamID == FGenericTeamId::NoTeam)
		{
			return ETeamAttitude::Neutral;
		}
		else
		{
			return ETeamAttitude::Hostile;
		}
	}

	return ETeamAttitude::Neutral;
}

void AMAIControllerBase::SetBehaviorTree(AMMonsterBase* Monster)
{
	UMMonsterDefinition* Def = Monster->GetMonsterDefinition();
	if (Def && Def->GetBehaviorTree())
	{
		RunBehaviorTree(Def->GetBehaviorTree());
	}
	else
	{
		MCHAE_ERROR("Monster BehaviorTree is not valid!!!");
	}

	AActor* MonsterSpawner = Monster->GetMonsterSpawner();
	if (MonsterSpawner)
	{
		Blackboard->SetValueAsVector(BBKey::ORIGIN_LOCATION, MonsterSpawner->GetActorLocation());
	}
}

void AMAIControllerBase::BindTargetDeathCallback()
{
	if (AbilitySystemComponent && !DelegateHandle_TargetLiftStateChange.IsValid())
	{
		DelegateHandle_TargetLiftStateChange = AbilitySystemComponent->AddGameplayEventTagContainerDelegate(
			FGameplayTagContainer(FPMGameplayTags::Get().GameplayEvent_Character_State_Dead), 
			FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &AMAIControllerBase::OnTargetDead));
	}
}

void AMAIControllerBase::RemoveTargetDeathCallback()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(
			FGameplayTagContainer(FPMGameplayTags::Get().GameplayEvent_Character_State_Dead),
			DelegateHandle_TargetLiftStateChange);
		DelegateHandle_TargetLiftStateChange.Reset();
	}
}

void AMAIControllerBase::OnTargetDead(FGameplayTag Tag, const FGameplayEventData* EventData)
{
	PerceptionComponent->ForgetAll();
	RemoveTargetDeathCallback();
	OnTargetForgotDelegated(nullptr);
}
