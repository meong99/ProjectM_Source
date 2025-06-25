// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "MGameplayAbilitySpec.generated.h"

USTRUCT(BlueprintType)
struct FMGameplayAbilitySpec : public FGameplayAbilitySpec
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 AdditiveInfo = INDEX_NONE;
};
