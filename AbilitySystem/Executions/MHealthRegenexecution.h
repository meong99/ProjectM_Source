// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "MHealthRegenexecution.generated.h"

/**
 *
 */
UCLASS()
class PROJECTM_API UMHealthRegenexecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UMHealthRegenexecution();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
