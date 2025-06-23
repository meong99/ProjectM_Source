// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MBTTask_ForgetTargetActor.generated.h"

/**
 *
 */
UCLASS()
class PROJECTM_API UMBTTask_ForgetTargetActor : public UBTTaskNode
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UMBTTask_ForgetTargetActor();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
/*
* Member Functions
*/
public:

/*
* Member Variables
*/
private:
};
