#pragma once

#include "CoreMinimal.h"
#include "Inventory/PMInventoryItemInstance.h"
#include "MConsumableItemInstance.generated.h"

class UPMAbilitySystemComponent;

/**
 *
 */
UCLASS(BlueprintType, Blueprintable)
class PROJECTM_API UMConsumableItemInstance : public UPMInventoryItemInstance
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UMConsumableItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual bool	ActivateItem() override;
	virtual bool	CanUseItem() const override;

/*
* Member Functions
*/
public:

private:
	UPMAbilitySystemComponent* GetAbilitySystemComponent() const;
/*
* Member Variables
*/
private:
};