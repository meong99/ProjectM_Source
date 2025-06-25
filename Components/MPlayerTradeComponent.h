// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MTradeComponentBase.h"
#include "MPlayerTradeComponent.generated.h"

/**
 *
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class PROJECTM_API UMPlayerTradeComponent : public UMTradeComponentBase
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UMPlayerTradeComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Server_OnRequestSimpleDataGrant_Implementation(AActor* Requestor, const FMTradeRequest& Request) override;
	virtual void Server_OnRequestSimpleTrading_Implementation(AActor* Requestor, const FMTradeRequest& Request) override;

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
