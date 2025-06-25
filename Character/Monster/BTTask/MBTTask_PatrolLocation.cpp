#include "MBTTask_PatrolLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "Types/MBlackboardTypes.h"

UMBTTask_PatrolLocation::UMBTTask_PatrolLocation()
{
	NodeName = TEXT("Find Random Location");
}

EBTNodeResult::Type UMBTTask_PatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = Cast<AAIController>(OwnerComp.GetAIOwner());

	if (IsValid(AIController) == false)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();
	const APawn* AIPawn = AIController->GetPawn();

	if (IsValid(AIPawn) == false || IsValid(Blackboard) == false)
	{
		return EBTNodeResult::Failed;
	}

	FVector origin = Blackboard->GetValueAsVector(BBKey::ORIGIN_LOCATION);
	if (origin == FVector::ZeroVector)
	{
		origin = AIPawn->GetActorLocation();
	}

	const UNavigationSystemV1* navSystem = UNavigationSystemV1::GetCurrent(GetWorld());

	if (IsValid(navSystem) == false)
	{
		return EBTNodeResult::Failed;
	}

	FNavLocation location;

	if (navSystem->GetRandomPointInNavigableRadius(origin, SearchRadius, location))
	{
		Blackboard->SetValueAsVector(BBKey::DESTINATION, location.Location);

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
