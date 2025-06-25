#include "PMWeaponUserInterface.h"
#include "Equipment/PMEquipmentManagerComponent.h"
#include "Weapons/PMWeaponInstance.h"

UPMWeaponUserInterface::UPMWeaponUserInterface(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{}

void UPMWeaponUserInterface::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}