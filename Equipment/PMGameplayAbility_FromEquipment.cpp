// Fill out your copyright notice in the Description page of Project Settings.


#include "PMGameplayAbility_FromEquipment.h"
#include "GameplayAbilitySpec.h"
#include "Inventory/PMInventoryItemInstance.h"
#include "Item/Equipment/MEquipableItemInstance.h"

UPMGameplayAbility_FromEquipment::UPMGameplayAbility_FromEquipment()
{

}

UMEquipableItemInstance* UPMGameplayAbility_FromEquipment::GetAssociatedEquipment() const
{
	if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		return Cast<UMEquipableItemInstance>(Spec->SourceObject.Get());
	}

	return  nullptr;
}

UPMInventoryItemInstance* UPMGameplayAbility_FromEquipment::GetAssociatedItem() const
{
	if (UMEquipableItemInstance* Equipment = GetAssociatedEquipment())
	{
		return Cast<UPMInventoryItemInstance>(Equipment->GetInstigator());
	}

	return nullptr;
}
