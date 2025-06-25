// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MTradeComponentBase.h"
#include "MNpcTradeComponent.generated.h"

/**
 *
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class PROJECTM_API UMNpcTradeComponent : public UMTradeComponentBase
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UMNpcTradeComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

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
