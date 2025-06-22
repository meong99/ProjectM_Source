#include "MItemTileWidget.h"
#include "Inventory/PMInventoryItemList.h"
#include "Inventory/PMInventoryItemDefinition.h"
#include "Inventory/PMInventoryItemInstance.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "UI/Inventory/MInventoryWidget.h"
#include "Blueprint/DragDropOperation.h"
#include "Inventory/PMInventoryManagerComponent.h"
#include "Components/TextBlock.h"
#include "CommonWidgets/MTileView.h"
#include "Kismet/GameplayStatics.h"
#include "PMGameplayTags.h"
#include "UI/Item/MContextableItemWidget.h"
#include "UI/MDragableWidget.h"

/*
* UMItemTileWidget
*/
UMItemTileWidget::UMItemTileWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{}

void UMItemTileWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
#ifndef WITH_EDITOR
	if (Index)
	{
		Index->SetVisibility(ESlateVisibility::Hidden);
	}
#endif // WITH_EDITOR

	UMItemDetailData* ItemData = Cast<UMItemDetailData>(ListItemObject);
	if (ItemData == nullptr)
	{
		return;
	}

	if (ItemData->SlotIndex != INDEX_NONE)
	{
		SetSlotIndex(ItemData->SlotIndex);
	}
	else
	{
		MCHAE_FETAL("Item slot index is not valid! Must be set slot index!");
	}

	if (ItemData->ItemEntry.IsValid())
	{
		ItemData->InitDelegate(ItemData->ItemEntry);
	}

	OwnerWidget = Cast<UMTileView>(ItemData->GetOuter());
	if (OwnerWidget == nullptr)
	{
		MCHAE_FETAL("Owner of TileWidget is must be MTileView! Check where the tileview is created.");
	}

	if (ItemImage)
	{
		ItemImage->SetDesiredSizeOverride({ ItemData->EntryWidth, ItemData->EntryHeight });
	}

	UpdateItemData();
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
}

void UMItemTileWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	if (ItemHandle.IsValid())
	{
		Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	}
}

bool UMItemTileWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (InOperation == nullptr)
	{
		MCHAE_WARNING("Operation is null");
		return false;
	}

	UMItemTileWidget* Other = Cast<UMItemTileWidget>(InOperation->Payload);
	if (Other == nullptr)
	{
		MCHAE_WARNING("payload is not ItemTileWidget!");
		return false;
	}
	else if (Other == this)
	{
		return false;
	}

	UMItemDetailData* MyItemDatail = GetListItem<UMItemDetailData>();
	UMItemDetailData* OtherItemDatail = Other->GetListItem<UMItemDetailData>();
	if (!MyItemDatail || !OtherItemDatail)
	{
		return false;
	}
	if (MyItemDatail->SlotType != OtherItemDatail->SlotType)
	{
		return false;
	}

	SwapItemData(Other);
	UpdateItemData();
	Other->UpdateItemData();

	return true;
}

void UMItemTileWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	MCHAE_LOG("on cancelled");
}

void UMItemTileWidget::UpdateItemData()
{
	UMItemDetailData* ItemDatail = GetListItem<UMItemDetailData>();
	if (ItemDatail == nullptr)
	{
		return;
	}

	const FPMInventoryEntry& NewItemEntry = ItemDatail->ItemEntry;
	if (NewItemEntry.IsValid() == false)
	{
		ResetItemSlot();
	}
	else
	{
		const UPMInventoryItemDefinition* ItemDef = GetDefault<UPMInventoryItemDefinition>(NewItemEntry.Instance->ItemDef);
		ItemImage->SetBrushFromTexture(ItemDef->ItemIcon);
		ItemImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		ItemHandle = NewItemEntry.GetItemHandle();
		ItemRowId = NewItemEntry.GetItemRowId();

		SetItemNum(NewItemEntry);
	}

	if (OwnerWidget)
	{
		OwnerWidget->UpdateEmptySlot(SlotIndex);
	}
}

void UMItemTileWidget::SetSlotIndex(const int32 InIndex)
{
	SlotIndex = InIndex;
	if (Index)
	{
		Index->SetText(FText::FromString(FString::FromInt(SlotIndex)));
	}
}

void UMItemTileWidget::ResetItemSlot()
{
	ItemImage->SetBrushFromTexture(nullptr);
	ItemImage->SetVisibility(ESlateVisibility::Hidden);

	ItemHandle.ItemUid = INDEX_NONE;
	ItemNum->SetVisibility(ESlateVisibility::Collapsed);
	ItemRowId = INDEX_NONE;
}

void UMItemTileWidget::SetItemNum(const FPMInventoryEntry& NewItemEntry)
{
	if (ItemNum && NewItemEntry.Instance)
	{
		int32 ItemQuentity = NewItemEntry.Instance->GetStatTagStackCount(FPMGameplayTags::Get().Item_Quentity);
		if (NewItemEntry.Instance->GetItemType() != EMItemType::Equipment && ItemQuentity > 0)
		{
			ItemNum->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			ItemNum->SetText(FText::AsNumber(ItemQuentity));
		}
		else
		{
			ItemNum->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	else
	{
		ItemNum->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMItemTileWidget::SwapItemData(UMItemTileWidget* Other)
{
	if (Other)
	{
		UMItemDetailData* OtherData = Other->GetListItem<UMItemDetailData>();
		UMItemDetailData* MyData = GetListItem<UMItemDetailData>();

		if (MyData && OtherData)
		{
			MyData->SwapEntry(OtherData);
		}
	}
}

void UMItemTileWidget::OnItemDoubleClick()
{
	if (ItemHandle.IsValid())
	{
		APlayerController* Controller = GetOwningPlayer();
		UPMInventoryManagerComponent* InventoryManagerComp = Controller ? Controller->FindComponentByClass<UPMInventoryManagerComponent>() : nullptr;
		if (InventoryManagerComp)
		{
			InventoryManagerComp->Server_UseItem(ItemHandle);
		}
		else
		{
			MCHAE_WARNING("Can't found InventoryComponent!");
		}
	}
}

UMItemDetailData* UMItemTileWidget::GetItemDatailData() const
{
	return GetListItem<UMItemDetailData>();
}
