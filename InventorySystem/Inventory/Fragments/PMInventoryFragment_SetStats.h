// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Inventory/PMInventoryItemDefinition.h"
#include "PMInventoryFragment_SetStats.generated.h"

class UPMInventoryItemInstance;

/**
 * 
 */
UCLASS()
class PROJECTM_API UPMInventoryFragment_SetStats : public UPMInventoryItemFragment
{
	GENERATED_BODY()
public:
	virtual void OnInstanceCreated(UPMInventoryItemInstance* Instance) const override;

	/** InitialItemStats gives constructor's parameters for PMGameplayTagStackContainer */
	UPROPERTY(EditDefaultsOnly, Category = Equipment)
	TMap<FGameplayTag, int32> InitialItemStats;
};
