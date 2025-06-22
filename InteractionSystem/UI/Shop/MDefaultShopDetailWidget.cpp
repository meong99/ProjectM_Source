#include "MDefaultShopDetailWidget.h"
#include "Components/VerticalBox.h"
#include "MDefaultShopSlotWidget.h"
#include "Interaction/MInteractiveAction_OpenShop.h"
#include "Components/TextBlock.h"
#include "Inventory/PMInventoryManagerComponent.h"

UMDefaultShopDetailWidget::UMDefaultShopDetailWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMDefaultShopDetailWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Type == EMShopDetailType::Shop)
	{
		TitleText->SetText(FText::FromString(TEXT("판매 가능")));
	}
	else
	{
		TitleText->SetText(FText::FromString(TEXT("판매 물품")));
	}
}

void UMDefaultShopDetailWidget::SetWidgetInfo(const FMWidgetInfo& InWidgetInfo)
{
	Super::SetWidgetInfo(InWidgetInfo);
	UMInteractiveAction_OpenShop* ShopAction = Cast<UMInteractiveAction_OpenShop>(WidgetInfo.WidgetInstigator);
	if (!ShopAction)
	{
		return;
	}

	ShopDefinition = ShopAction->GetShopDefinition();
	if ((ShopDefinition.ShopItemRowids.Num() == 0 && Type == EMShopDetailType::Shop) || !ShopDefinition.SlotClass)
	{
		return;
	}

	SlotVerticalBox->ClearChildren();
	if (Type == EMShopDetailType::Shop)
	{
		InitShopDetail();
	}
	else
	{
		InitInventoryDetail();

		UPMInventoryManagerComponent* InvenManager = GetOwningPlayer() ? GetOwningPlayer()->FindComponentByClass<UPMInventoryManagerComponent>() : nullptr;
		if (InvenManager)
		{
			NewItemDelegateHandle = InvenManager->Delegate_OnItemIncreased.AddWeakLambda(this, [this](const FMItemResponse& ItemRespons)->void
			{
				if (ItemRespons.ResponsType == EMItemResponseType::TotallyNewItem)
				{
					CreateSlot(ItemRespons.ItemRequest.ItemRowId, ItemRespons.ItemRequest.ItemHandle);
				}
			});
		}
	}
}

void UMDefaultShopDetailWidget::NativeDestruct()
{
	UPMInventoryManagerComponent* InvenManager = GetOwningPlayer() ? GetOwningPlayer()->FindComponentByClass<UPMInventoryManagerComponent>() : nullptr;
	if (InvenManager && NewItemDelegateHandle.IsValid())
	{
		InvenManager->Delegate_OnItemIncreased.Remove(NewItemDelegateHandle);
	}

	NewItemDelegateHandle.Reset();
}

void UMDefaultShopDetailWidget::InitShopDetail()
{
	CreateSlotsFromRowIds(ShopDefinition.ShopItemRowids);
}

void UMDefaultShopDetailWidget::InitInventoryDetail()
{
	TArray<FPMInventoryEntry> ItemEntries;

	UPMInventoryManagerComponent* InvenManager = GetOwningPlayer() ? GetOwningPlayer()->FindComponentByClass<UPMInventoryManagerComponent>() : nullptr;
	if (InvenManager)
	{
		const FPMInventoryItemList& ConsumableItemList = InvenManager->GetConsumableItemList();
		for (const FPMInventoryEntry& Entry : ConsumableItemList.Entries)
		{
			if (Entry.IsValid())
			{
				ItemEntries.Add(Entry);
			}
		}

		const FPMInventoryItemList& EquipmentItemList = InvenManager->GetEquipmentItemList();
		for (const FPMInventoryEntry& Entry : EquipmentItemList.Entries)
		{
			if (Entry.IsValid())
			{
				ItemEntries.Add(Entry);
			}
		}

		const FPMInventoryItemList& MiscellaneousItemList = InvenManager->GetMiscellaneousItemList();
		for (const FPMInventoryEntry& Entry : MiscellaneousItemList.Entries)
		{
			if (Entry.IsValid())
			{
				ItemEntries.Add(Entry);
			}
		}
	}

	for (const FPMInventoryEntry& Entry : ItemEntries)
	{
		CreateSlot(Entry.GetItemRowId(), Entry.GetItemHandle());
	}
}

void UMDefaultShopDetailWidget::CreateSlotsFromRowIds(const TArray<int32>& ItemRowIds)
{
	for (const int32 RowId : ItemRowIds)
	{
		CreateSlot(RowId);
	}
}

void UMDefaultShopDetailWidget::CreateSlot(const int32 ItemRowId, const FMItemHandle& ItemHandle)
{
	UMDefaultShopSlotWidget* SlotWidget = CreateWidget<UMDefaultShopSlotWidget>(GetOwningPlayer(), ShopDefinition.SlotClass);
	if (!SlotWidget)
	{
		ensure(false);
		return;
	}
	SlotWidget->SetWidgetInfo(WidgetInfo);
	SlotWidget->InitSlot(ItemRowId, Type, ItemHandle);
	SlotVerticalBox->AddChildToVerticalBox(SlotWidget);
}
