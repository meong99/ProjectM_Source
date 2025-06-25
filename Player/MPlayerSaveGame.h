// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Templates/SubclassOf.h"
#include "MPlayerSaveGame.generated.h"

class UPMInventoryItemDefinition;

/**
 * 
 */
UCLASS()
class PROJECTM_API UMPlayerSaveGame : public USaveGame
{
	GENERATED_BODY()
	
/*
* Overrided Functions
*/
public:
	UMPlayerSaveGame();
/*
* Member Functions
*/
public:

/*
* Member Variables
*/
public:
	UPROPERTY()
	FString SaveSlotName;

	UPROPERTY()
	int32 SaveIndex;

	UPROPERTY()
	TArray<TSubclassOf<UPMInventoryItemDefinition>> ItemDefinitions;

	UPROPERTY()
	TSubclassOf<UPMInventoryItemDefinition> EquippedItem;
};
