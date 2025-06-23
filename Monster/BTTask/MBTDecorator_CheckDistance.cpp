#include "MBTDecorator_CheckDistance.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Types/MBlackboardTypes.h"

UMBTDecorator_CheckDistance::UMBTDecorator_CheckDistance()
{
	NodeName = TEXT("CheckDistanceFromOrigin");
}

bool UMBTDecorator_CheckDistance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool Ret = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	AAIController* AIController = Cast<AAIController>(OwnerComp.GetAIOwner());

	if (IsValid(AIController) == false)
	{
		return false;
	}

	UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();
	const APawn* AIPawn = AIController->GetPawn();

	if (IsValid(AIPawn) == false || IsValid(Blackboard) == false)
	{
		return false;
	}

	FVector origin = Blackboard->GetValueAsVector(BBKey::ORIGIN_LOCATION);

	if (FVector::Distance(origin, AIPawn->GetActorLocation()) > MaxDistance)
	{
		Ret = false;
		return Ret == bIsSet;
	}

	return Ret == bIsSet;
}
