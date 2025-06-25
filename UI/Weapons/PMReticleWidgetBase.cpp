#include "PMReticleWidgetBase.h"
#include "Weapons/PMWeaponInstance.h"
#include "Inventory/PMInventoryItemInstance.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(PMReticleWidgetBase)

UPMReticleWidgetBase::UPMReticleWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void UPMReticleWidgetBase::InitializeFromWeapon(UPMWeaponInstance* InWeapon)
{
	WeaponInstance = InWeapon;
	InventoryInstance = nullptr;
	if (WeaponInstance)
	{
		InventoryInstance = Cast<UPMInventoryItemInstance>(WeaponInstance->GetInstigator());
	}

	OnWeaponInitialized();
}