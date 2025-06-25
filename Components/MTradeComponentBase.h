// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/MTradeType.h"
#include "MTradeComponentBase.generated.h"

/**
 *
 */
UCLASS(Abstract, BlueprintType, meta = (BlueprintSpawnableComponent))
class PROJECTM_API UMTradeComponentBase : public UActorComponent
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UMTradeComponentBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

/*
* Member Functions
*/
public:
	// Do Nothing. Have to overriding
	UFUNCTION(Server, Reliable)
	virtual void Server_OnRequestSimpleTrading(AActor* Requestor, const FMTradeRequest& Request);

	// Do Nothing. Have to overriding
	UFUNCTION(Server, Reliable)
	virtual void Server_OnRequestSimpleDataGrant(AActor* Requestor, const FMTradeRequest& Request);
	
	virtual void OnReceivedResponse(const FMTradeRequest& Request, const FMTradeResponse& Response) {}

protected:
	const FMTradeResponse MakeErrorResponse(const FMTradeRequest& Request, const FString& ContextString = TEXT(""));
	const FMTradeResponse MakeFailResponse(const FMTradeRequest& Request, const FString& ContextString = TEXT(""));
	const FMTradeResponse MakeSuccessResponse(const FMTradeRequest& Request, const FString& ContextString = TEXT(""));

/*
* Member Variables
*/
protected:
	
};
