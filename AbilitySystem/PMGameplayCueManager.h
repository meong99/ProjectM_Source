// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueManager.h"
#include "PMGameplayCueManager.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTM_API UPMGameplayCueManager : public UGameplayCueManager
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UPMGameplayCueManager();

	static UPMGameplayCueManager* Get();

/*
* Member Functions
*/
public:
	void RefreshGameplayCuePrimaryAsset();


/*
* Member Variables
*/
public:
};
