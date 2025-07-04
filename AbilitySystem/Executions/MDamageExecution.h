// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "MDamageExecution.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTM_API UMDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UMDamageExecution();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
