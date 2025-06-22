// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MWidgetBase.h"
#include "Types/MShopTypes.h"
#include "Inventory/MInventoryTypes.h"
#include "MDefaultShopDetailWidget.generated.h"

class UMDefaultShopSlotWidget;
class UVerticalBox;
class UTextBlock;

UCLASS(Abstract)
class PROJECTM_API UMDefaultShopDetailWidget : public UMWidgetBase
{
	GENERATED_BODY()

	/*
	* Overrided Functions
	*/
public:
	UMDefaultShopDetailWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeOnInitialized() override;
	virtual void SetWidgetInfo(const FMWidgetInfo& InWidgetInfo) override;
	virtual void NativeDestruct() override;

		/*
	* Member Functions
	*/
public:

protected:
	void InitShopDetail();
	void InitInventoryDetail();
	void CreateSlotsFromRowIds(const TArray<int32>& ItemRowIds);
	void CreateSlot(const int32 ItemRowId, const FMItemHandle& ItemHandle = {});

	/*
	* Member Variables
	*/

protected:
	FDelegateHandle NewItemDelegateHandle;

	UPROPERTY(EditAnywhere, Category="ProjectM")
	EMShopDetailType Type = EMShopDetailType::Shop;

	UPROPERTY(BlueprintReadOnly)
	FMShopDefinition ShopDefinition;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="ProjectM")
	TObjectPtr<UVerticalBox> SlotVerticalBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UTextBlock> TitleText;
};
