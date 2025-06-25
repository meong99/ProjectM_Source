#include "MConsumableItemDefinition.h"
#include "MConsumableItemInstance.h"

UMConsumableItemDefinition::UMConsumableItemDefinition()
{
	InstanceType = UMConsumableItemInstance::StaticClass();
}
