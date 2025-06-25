// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MBTTask_PatrolLocation.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTM_API UMBTTask_PatrolLocation : public UBTTaskNode
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UMBTTask_PatrolLocation();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
/*
* Member Functions
*/
public:

/*
* Member Variables
*/
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectM | PatrolLocation", meta = (AllowPrivateAccess = true))
    float SearchRadius = 2000.f;
};
