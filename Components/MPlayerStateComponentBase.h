// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PlayerStateComponent.h"
#include "GameFramework/PlayerState.h"
#include "MPlayerStateComponentBase.generated.h"

class UPMExperienceDefinition;
class UAbilitySystemComponent;

/**
 *
 */
UCLASS(BlueprintType)
class PROJECTM_API UMPlayerStateComponentBase : public UPlayerStateComponent
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UMPlayerStateComponentBase(const FObjectInitializer& ObjectInitializer);
	virtual void OnExperienceLoaded(const UPMExperienceDefinition* CurrentExperience){}
	virtual void OnSetNewPawn(APawn* NewPawn, APawn* OldPawn){}

/*
* Member Functions
*/
public:
	template <class T = APlayerController>
	T* GetPlayerController() const
	{
		APlayerState* PlayerState = GetPlayerState<APlayerState>();
		if (PlayerState)
		{
			return Cast<T>(PlayerState->GetPlayerController());
		}

		return nullptr;
	}

	template <class T = UAbilitySystemComponent>
	T* GetAbilitySystemComponent() const
	{
		APlayerState* PlayerState = GetPlayerState<APlayerState>();
		if (PlayerState)
		{
			return PlayerState->FindComponentByClass<T>();
		}

		return nullptr;
	}
protected:
/*
* Member Variables
*/
protected:
	
};
