#include "MBTTask_ForgetTargetActor.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "Types/MBlackboardTypes.h"
#include "Perception/AIPerceptionComponent.h"

UMBTTask_ForgetTargetActor::UMBTTask_ForgetTargetActor()
{
	NodeName = TEXT("ForgetTargetActor");
}

EBTNodeResult::Type UMBTTask_ForgetTargetActor::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = Cast<AAIController>(OwnerComp.GetAIOwner());

	if (IsValid(AIController) == false)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();

	if (IsValid(Blackboard) == false)
	{
		return EBTNodeResult::Failed;
	}

	UAIPerceptionComponent* AIPerception = AIController->GetAIPerceptionComponent();
	if (IsValid(AIPerception) == false)
	{
		return EBTNodeResult::Failed;
	}

	Blackboard->SetValueAsObject(BBKey::TARGET_ACTOR, nullptr);
	Blackboard->SetValueAsBool(BBKey::FOUND_PLAYER, false);

	return EBTNodeResult::Succeeded;
}
