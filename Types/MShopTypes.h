// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Templates/SubclassOf.h"
#include "MShopTypes.generated.h"

class UMDefaultShopSlotWidget;

USTRUCT(BlueprintType)
struct FMShopDefinition
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	int32 RowId = INDEX_NONE;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<int32> ShopItemRowids;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag ShopWidgetTag;
	
	UPROPERTY(EditAnywhere, Category="ProjectM")
	TSubclassOf<UMDefaultShopSlotWidget> SlotClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ShopName;
};

UENUM(BlueprintType)
enum class EMShopDetailType : uint8
{
	Shop,
	UserInventory,
};
