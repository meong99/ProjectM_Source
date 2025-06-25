// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "PMGameplayAbility.h"
#include "PMAbilityCost.generated.h"

/**
 * 
 */
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class PROJECTM_API UPMAbilityCost : public UObject
{
	GENERATED_BODY()
public:
	UPMAbilityCost();

	/**
	 * CheckCost and ApplyCost function signature come from UGameAbility's CheckCost and ApplyCost
	 * - You can think AbilityCost as managable-unit to check/apply cost for GameplayAbility
	 */
	virtual bool CheckCost(const UPMGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
	{
		return true;
	}

	virtual void ApplyCost(const UPMGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
	{
	}
};
