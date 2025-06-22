#include "MWeaponItemDefinition.h"
#include "Weapons/PMWeaponInstance.h"

UMWeaponItemDefinition::UMWeaponItemDefinition()
{
	EquipmentInstanceType = UPMWeaponInstance::StaticClass();
}
