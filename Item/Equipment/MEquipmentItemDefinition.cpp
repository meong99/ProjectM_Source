#include "MEquipmentItemDefinition.h"
#include "MEquipableItemInstance.h"
#include "MEquipmentItemInstance.h"

UMEquipmentItemDefinition::UMEquipmentItemDefinition()
{
	InstanceType = UMEquipableItemInstance::StaticClass();
	EquipmentInstanceType = UMEquipmentItemInstance::StaticClass();
}
