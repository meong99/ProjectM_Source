// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "MPawnComponentBase.generated.h"

/**
 *
 */
UCLASS(BlueprintType)
class PROJECTM_API UMPawnComponentBase : public UPawnComponent
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UMPawnComponentBase(const FObjectInitializer& ObjectInitializer);

/*
* Member Functions
*/
public:
	bool IsNetConnectionReady() const;
	virtual void PawnClientRestart(){}
	virtual void PossessedBy(AController* NewController){ }

protected:
/*
* Member Variables
*/
protected:
	
};
