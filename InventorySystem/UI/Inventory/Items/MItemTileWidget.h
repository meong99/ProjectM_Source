	// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "UI/MDragableWidget.h"

#include "Inventory/PMInventoryItemList.h"
#include "MItemTileWidget.generated.h"

class UTileView;
class UImage;
class UMInventoryWidget;
class UTextBlock;
class UMTileView;
class UMItemDetailData;

UCLASS(BlueprintType)
class PROJECTM_API UMItemTileWidget : public UMDragableWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UMItemTileWidget(const FObjectInitializer& ObjectInitializer);

	virtual void	NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void	NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool	NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void	NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

/*
* Member Functions
*/
public:
	void SwapItemData(UMItemTileWidget* Other);
	void OnItemDoubleClick();

	UFUNCTION(BlueprintCallable)
	UMItemDetailData* GetItemDatailData() const;

protected:
	virtual void UpdateItemData();
	void SetSlotIndex(const int32 InIndex);
	void ResetItemSlot();
	void SetItemNum(const FPMInventoryEntry& NewItemEntry);

/*
* Member Variables
*/
protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMTileView> OwnerWidget;

	UPROPERTY(BlueprintReadOnly)
	int32 SlotIndex = INDEX_NONE;
	
 	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UTextBlock> ItemNum;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> Index;

	UPROPERTY(BlueprintReadOnly)
	FMItemHandle ItemHandle;
};
