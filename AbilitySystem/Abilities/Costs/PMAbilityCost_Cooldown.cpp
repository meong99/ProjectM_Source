// Fill out your copyright notice in the Description page of Project Settings.

#include "PMAbilityCost_Cooldown.h"
#include "NativeGameplayTags.h"
#include "Equipment/PMGameplayAbility_FromEquipment.h"
#include "Inventory/PMInventoryItemInstance.h"
#include "Engine/World.h"
#include "AbilitySystem/Abilities/PMGameplayAbility.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_ABILITY_FAIL_COOLDOWN, "Ability.ActivateFail.Cooldown")

UPMAbilityCost_Cooldown::UPMAbilityCost_Cooldown() : Super()
{
	Cooldown.SetValue(1.0f);
	FailureTag = TAG_ABILITY_FAIL_COOLDOWN;
}

bool UPMAbilityCost_Cooldown::CheckCost(const UPMGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (Ability)
	{
		const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);
		const float CooldownValue = Cooldown.GetValueAtLevel(AbilityLevel);
		const float CurrentTime = GetCurrentTime();
		const bool	bCanApplyCost = CurrentTime < CooldownValue || CurrentTime - StartTime > CooldownValue;

		return bCanApplyCost;
	}
	else
	{
		// 어빌리티가 없는데?
		ensure(false);
	}

	return false;
}

void UPMAbilityCost_Cooldown::ApplyCost(const UPMGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	StartTime = GetCurrentTime();
}

float UPMAbilityCost_Cooldown::GetCurrentTime() const
{
	UWorld* World = GetWorld();
	if (World)
	{
		return World->GetTimeSeconds();
	}

	return 0.0f;
}
