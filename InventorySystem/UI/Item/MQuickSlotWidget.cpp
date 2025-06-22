#include "MQuickSlotWidget.h"
#include "Inventory/PMInventoryManagerComponent.h"
#include "Player/PMPlayerControllerBase.h"
#include "CommonWidgets/MTileView.h"

UMQuickSlotWidget::UMQuickSlotWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMQuickSlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	QuickSlotView->InitView(4);
	BindDelegates();
}

void UMQuickSlotWidget::BindDelegates()
{
	APMPlayerControllerBase* PlayerController = Cast<APMPlayerControllerBase>(GetOwningPlayer());
	InventoryComponent = PlayerController ? PlayerController->FindComponentByClass<UPMInventoryManagerComponent>() : nullptr;
	if (IsValid(InventoryComponent))
	{
		InventoryComponent->Delegate_OnRemoveItem.AddUObject(this, &UMQuickSlotWidget::Callback_RemoveItem);
	}
	else
	{
		ensure(false);
		MCHAE_ERROR("Can't get inventorycomponent!");
	}
}

void UMQuickSlotWidget::Callback_RemoveItem(const FMItemResponse& ItemRespons)
{
	if (ItemRespons.ResponsType == EMItemResponseType::Removed && ItemRespons.ItemRequest.ItemInstance)
	{
		if (QuickSlotView)
		{
			QuickSlotView->RemoveItem(ItemRespons.ItemRequest.ItemHandle);
		}
	}
}
