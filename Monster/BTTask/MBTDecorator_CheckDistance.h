// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "MBTDecorator_CheckDistance.generated.h"

/**
 *
 */
UCLASS()
class PROJECTM_API UMBTDecorator_CheckDistance : public UBTDecorator
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UMBTDecorator_CheckDistance();
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
/*
* Member Functions
*/
public:

/*
* Member Variables
*/
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectM", meta = (AllowPrivateAccess = true))
    float MaxDistance = 4000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectM", meta = (AllowPrivateAccess = true))
	bool bIsSet = true;
};
