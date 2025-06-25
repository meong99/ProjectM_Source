// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/PMAbilityCost.h"
#include "GameplayTagContainer.h"
#include "PMAbilityCost_Cooldown.generated.h"

/**
 *
 */
UCLASS(meta = (DisplayName = "Cooldown"))
class PROJECTM_API UPMAbilityCost_Cooldown : public UPMAbilityCost
{
	GENERATED_BODY()
public:
	UPMAbilityCost_Cooldown();

	virtual bool CheckCost(const UPMGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ApplyCost(const UPMGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	float	GetCurrentTime() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Costs)
	FScalableFloat Cooldown;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Costs)
	FGameplayTag Tag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Costs)
	FGameplayTag FailureTag;

	float StartTime = 0.0f;
};
