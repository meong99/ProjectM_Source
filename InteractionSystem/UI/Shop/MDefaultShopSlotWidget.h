// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MWidgetBase.h"
#include "Types/MShopTypes.h"
#include "Inventory/MInventoryTypes.h"
#include "MDefaultShopSlotWidget.generated.h"

class UImage;
class UTextBlock;
class UPMInventoryItemDefinition;
class UMWalletComponent;
class UWidgetSwitcher;
class UMContextableItemWidget;
struct FPMInventoryEntry;

UCLASS(Abstract)
class PROJECTM_API UMDefaultShopSlotWidget : public UMWidgetBase
{
	GENERATED_BODY()

	/*
	* Overrided Functions
	*/
public:
	UMDefaultShopSlotWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;

	/*
	* Member Functions
	*/
public:
	void InitSlot(const int32 InRowId, EMShopDetailType InType, const FMItemHandle& ItemHandle = {});

protected:
	void SetShopSlot();
	void SetUserSlot(const FMItemHandle& ItemHandle);

	void SetPrice(int32 NewPrice);

	void OnClickItem_Buy();
	void OnClickItem_Sell();
	void Callback_OnChangeItem(const FMItemResponse& ItemRespons);
	void Callback_OnRemoveItem(const FMItemResponse& ItemRespons);

	/*
	* Member Variables
	*/
protected:
	int32 RowId = INDEX_NONE;

	EMShopDetailType Type = EMShopDetailType::Shop;

	TWeakObjectPtr<UMWalletComponent> PlayerWalletComp;

	FMItemHandle		CachedItemHandle;
	FDelegateHandle		OnChangeHandle;
	FDelegateHandle		OnRemoveHandle;

	UPROPERTY()
	TObjectPtr<UPMInventoryItemDefinition> ItemCDO;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UImage> ItemIconBackground;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UMContextableItemWidget> ItemIcon;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UTextBlock> ItemName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UTextBlock> ItemCount;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UImage> PriceImage;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="ProjectM")
	TObjectPtr<UTextBlock> Price;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="ProjectM")
	TObjectPtr<UTextBlock> NotEnoughPrice;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="ProjectM")
	UWidgetSwitcher* PriceWidgetSwitcher;
};
