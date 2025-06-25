// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ControllerComponent.h"
#include "MControllerComponentBase.generated.h"

class UPMExperienceDefinition;
class APawn;

/**
 *
 */
UCLASS(BlueprintType)
class PROJECTM_API UMControllerComponentBase : public UControllerComponent
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UMControllerComponentBase(const FObjectInitializer& ObjectInitializer);

/*
* Member Functions
*/
public:
	virtual void OnServerRestartPlayer() {}
	virtual void OnClientRestart(APawn* NewPawn) {}
	virtual void OnExperienceLoaded(const UPMExperienceDefinition* LoadedExperienceDefinition){}
	virtual void OnPossess(APawn* aPawn) {}

protected:
/*
* Member Variables
*/
protected:
	
};
