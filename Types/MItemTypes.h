// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MAbilityTypes.h" // 제거
#include "MItemTypes.generated.h"

class UGameplayEffect;

UENUM(BlueprintType)
enum class EMEquipmentItemType : uint8
{
	None UMETA(DisplayName = "None"),
	Weapon UMETA(DisplayName = "Weapon"),
};
