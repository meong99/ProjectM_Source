#pragma once

#include "CommonUserWidget.h"
#include "UObject/ObjectPtr.h"
#include "UObject/UObjectGlobals.h"
#include "PMReticleWidgetBase.generated.h"

class UPMWeaponInstance;
class UPMInventoryItemInstance;

UCLASS(Abstract)
class UPMReticleWidgetBase : public UCommonUserWidget
{
	GENERATED_BODY()
public:
	UPMReticleWidgetBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	void InitializeFromWeapon(UPMWeaponInstance* InWeapon);

	UFUNCTION(BlueprintImplementableEvent)
	void OnWeaponInitialized();

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UPMWeaponInstance> WeaponInstance;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UPMInventoryItemInstance> InventoryInstance;
};