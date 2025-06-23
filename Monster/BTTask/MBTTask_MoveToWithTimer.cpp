// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Monster/BTTask/MBTTask_MoveToWithTimer.h"
#include "AIController.h"

UE_DISABLE_OPTIMIZATION

UMBTTask_MoveToWithTimer::UMBTTask_MoveToWithTimer()
{
	NodeName = "Move to target with timer";
}

void UMBTTask_MoveToWithTimer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	ElapsedTime += DeltaSeconds;

	if (MaximumTime && ElapsedTime >= MaximumTime)
	{
		ElapsedTime = 0;

		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}

EBTNodeResult::Type UMBTTask_MoveToWithTimer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	bNotifyTick = true;

	AcceptableRadius = 5;

	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	return Result;
}

void UMBTTask_MoveToWithTimer::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
										  EBTNodeResult::Type TaskResult)
{
	ElapsedTime = 0;
	bNotifyTick = false;

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}
UE_ENABLE_OPTIMIZATION
