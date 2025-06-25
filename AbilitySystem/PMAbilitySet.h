// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpecHandle.h"
#include "Types/MAbilityTypes.h"
#include "PMAbilitySet.generated.h"

class UPMGameplayAbility;
class UPMAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class PROJECTM_API UPMAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
/*
* Overrided Functions
*/
public:
	UPMAbilitySet();

/*
* Member Functions
*/
public:
	void GiveToAbilitySystem(UPMAbilitySystemComponent* AbilitySystemComp, OUT FMAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr, int32 AdditiveInfo = INDEX_NONE) const;

/*
* Member Variables
*/
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess), Category = "Gameplay Abilities")
	TArray<FPMAbilitySet_GameplayAbility> GrantedGameplayAbilities;
};
