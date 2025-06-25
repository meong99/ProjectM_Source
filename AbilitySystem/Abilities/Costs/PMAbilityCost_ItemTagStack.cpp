// Fill out your copyright notice in the Description page of Project Settings.

#include "PMAbilityCost_ItemTagStack.h"
#include "NativeGameplayTags.h"
#include "Equipment/PMGameplayAbility_FromEquipment.h"
#include "Inventory/PMInventoryItemInstance.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_ABILITY_FAIL_COST, "Ability.ActivateFail.Cost")

UPMAbilityCost_ItemTagStack::UPMAbilityCost_ItemTagStack() : Super()
{
	Quantity.SetValue(1.0f);
	FailureTag = TAG_ABILITY_FAIL_COST;
}

bool UPMAbilityCost_ItemTagStack::CheckCost(const UPMGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	// we only check the cost when item is equipped
	if (const UPMGameplayAbility_FromEquipment* EquipmentAbility = Cast<const UPMGameplayAbility_FromEquipment>(Ability))
	{
		if (UPMInventoryItemInstance* ItemInstance = EquipmentAbility->GetAssociatedItem())
		{
			const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

			// currently, it is just pistol (basic weapon)
			// to understand this a little bit deeply, we try to think weapon as magic pistol:
			// - the magic pistol costs two bullets, give a strong one shot when the weapon is lv2
			// - the magic pistol lv5 costs four bullets, give more strong one shot
			const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
			const int32 NumStacks = FMath::TruncToInt(NumStacksReal);
			const int32 TagStacks = ItemInstance->GetStatTagStackCount(Tag);
			const bool bCanApplyCost = TagStacks >= NumStacks;

			// when we cannot be afford to give a shot, leave the failure tag in OptionalRelevantTags:
			if (!bCanApplyCost && OptionalRelevantTags && FailureTag.IsValid())
			{
				OptionalRelevantTags->AddTag(FailureTag);
			}

			return bCanApplyCost;
		}
	}
	return false;
}

void UPMAbilityCost_ItemTagStack::ApplyCost(const UPMGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (const UPMGameplayAbility_FromEquipment* EquipmentAbility = Cast<const UPMGameplayAbility_FromEquipment>(Ability))
	{
		if (UPMInventoryItemInstance* ItemInstance = EquipmentAbility->GetAssociatedItem())
		{
			const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);
			const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
			const int32 NumStacks = FMath::TruncToInt(NumStacksReal);

			// decrease amount of stat tags in an inventory item instance
			ItemInstance->RemoveStatTagStack(Tag, NumStacks);
		}
	}
}