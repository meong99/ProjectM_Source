#pragma once

#include "Containers/Array.h"
#include "Templates/SubclassOf.h"
#include "Inventory/PMInventoryItemDefinition.h"
#include "InventoryFragment_ReticleConfig.generated.h"

/** forward declaration */
class UPMReticleWidgetBase;

UCLASS()
class UPMInventoryFragment_ReticleConfig : public UPMInventoryItemFragment
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Reticle)
	TArray<TSubclassOf<UPMReticleWidgetBase>> ReticleWidgets;
};