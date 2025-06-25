	// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MWidgetBase.h"
#include "Inventory/MInventoryTypes.h"
#include "MQuickSlotWidget.generated.h"

class UPMInventoryManagerComponent;
class UMTileView;

UCLASS(BlueprintType)
class PROJECTM_API UMQuickSlotWidget : public UMWidgetBase
{
	GENERATED_BODY()

	/*
	* Overrided Functions
	*/
public:
	UMQuickSlotWidget(const FObjectInitializer& ObjectInitializer);
	virtual void	NativeOnInitialized() override;

	/*
	* Member Functions
	*/
private:
	void	BindDelegates();
	void	Callback_RemoveItem(const FMItemResponse& ItemRespons);

/*
* Member Variables
*/
protected:
	UPROPERTY()
	UPMInventoryManagerComponent* InventoryComponent;

	UPROPERTY()
	TObjectPtr<UMTileView> QuickSlotView;
};
