#pragma once

#include "CoreMinimal.h"
#include "Inventory/PMInventoryItemDefinition.h"
#include "Templates/SubclassOf.h"
#include "Types/MItemTypes.h"
#include "MConsumableItemDefinition.generated.h"

class UGameplayEffect;
class UCurveTable;

UCLASS()
class PROJECTM_API UMConsumableItemDefinition : public UPMInventoryItemDefinition
{
	GENERATED_BODY()

public:
	UMConsumableItemDefinition();
};