// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/PMAbilityCost.h"
#include "PMAbilityCost_ItemTagStack.generated.h"

/**
 *
 */
UCLASS(meta = (DisplayName = "Item Tag Stack"))
class PROJECTM_API UPMAbilityCost_ItemTagStack : public UPMAbilityCost
{
	GENERATED_BODY()
public:
	UPMAbilityCost_ItemTagStack();

	virtual bool CheckCost(const UPMGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ApplyCost(const UPMGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	/**
	 * how much of the tag spend:
	 * - FScalableFloat is scaled by Curve with base float value
	 * - Curve is normally indexed by ability level
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Costs)
	FScalableFloat Quantity;

	/** gameplay tag combined with the cost */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Costs)
	FGameplayTag Tag;

	/** failure identifier with gameplay-tag */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Costs)
	FGameplayTag FailureTag;
};
