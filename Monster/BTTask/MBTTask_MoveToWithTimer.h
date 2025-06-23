// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "MBTTask_MoveToWithTimer.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTM_API UMBTTask_MoveToWithTimer : public UBTTask_MoveTo
{
    GENERATED_BODY()
		/*
* Overrided Function
*/
public:
    UMBTTask_MoveToWithTimer();

protected:
    virtual void                TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    virtual void                OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                               EBTNodeResult::Type TaskResult) override;

/*
* Member Functions
*/
public:

/*
* Member Variables
*/
protected:
    // 태스크가 실행될 수 있는 최대 시간
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BTTask")
    float MaximumTime = 5.f;

    // 태스크 시작 이후 경과된 시간
    float ElapsedTime = 0.f;
};