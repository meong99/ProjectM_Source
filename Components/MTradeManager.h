// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "MTradeManager.generated.h"

/**
 *
 */
UCLASS(BlueprintType)
class PROJECTM_API UMTradeManager : public UGameStateComponent
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UMTradeManager(const FObjectInitializer& ObjectInitializer);

/*
* Member Functions
*/
public:

protected:
/*
* Member Variables
*/
protected:
	
};
