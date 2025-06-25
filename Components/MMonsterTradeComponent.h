// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MTradeComponentBase.h"
#include "MMonsterTradeComponent.generated.h"

/**
 *
 */
UCLASS(BlueprintType)
class PROJECTM_API UMMonsterTradeComponent : public UMTradeComponentBase
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UMMonsterTradeComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void OnReceivedResponse(const FMTradeRequest& Request, const FMTradeResponse& Response) override;

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
