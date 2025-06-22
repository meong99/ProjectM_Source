#include "MDefaultShopSlotWidget.h"
#include "Inventory/PMInventoryItemDefinition.h"
#include "Engine/Engine.h"
#include "System/MDataTableManager.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "Components/MTradeManager.h"
#include "GameFramework/Actor.h"
#include "Components/MTradeComponentBase.h"
#include "Character/Components/MWalletComponent.h"
#include "Components/WidgetSwitcher.h"
#include "Inventory/PMInventoryManagerComponent.h"
#include "UI/Item/MContextableItemWidget.h"

UMDefaultShopSlotWidget::UMDefaultShopSlotWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

FReply UMDefaultShopSlotWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
	
	if (Type == EMShopDetailType::Shop)
	{
		OnClickItem_Buy();
	}
	else
	{
		OnClickItem_Sell();
	}

	return FReply::Handled();
}

void UMDefaultShopSlotWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (PlayerWalletComp.IsValid() && ItemCDO)
	{
		if (PlayerWalletComp->GetGold() < ItemCDO->BuyPrice && Type == EMShopDetailType::Shop)
		{
			PriceWidgetSwitcher->SetActiveWidget(NotEnoughPrice);
		}
		else
		{
			PriceWidgetSwitcher->SetActiveWidget(Price);
		}
	}
}

void UMDefaultShopSlotWidget::NativeDestruct()
{
	if (CachedItemHandle.IsValid())
	{
		APlayerController* PlayerController = GetOwningPlayer();
		UPMInventoryManagerComponent* InvenManager = PlayerController ? PlayerController->FindComponentByClass<UPMInventoryManagerComponent>() : nullptr;
		if (InvenManager)
		{
			InvenManager->RemoveDelegateOnChangeItemQuentity(CachedItemHandle.ItemUid, OnChangeHandle);
			InvenManager->Delegate_OnRemoveItem.Remove(OnRemoveHandle);
		}
	}

	CachedItemHandle.Reset();
	OnChangeHandle.Reset();
	OnRemoveHandle.Reset();
}

void UMDefaultShopSlotWidget::InitSlot(const int32 InRowId, EMShopDetailType InType, const FMItemHandle& ItemHandle)
{
	RowId = InRowId;
	Type = InType;
	ItemIcon->SetItemRowId(RowId);
	CachedItemHandle = ItemHandle;

	UMDataTableManager* TableManager = GEngine->GetEngineSubsystem<UMDataTableManager>();
	if (TableManager)
	{
		ItemCDO = TableManager->GetItemDefinition(RowId);
	}
	else
	{
		ensure(false);
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	if (Type == EMShopDetailType::Shop)
	{
		SetShopSlot();
	}
	else
	{
		SetUserSlot(ItemHandle);
	}

	APlayerController* PlayerController = GetOwningPlayer();
	if (PlayerController)
	{
		PlayerWalletComp = PlayerController->FindComponentByClass<UMWalletComponent>();
	}
}

void UMDefaultShopSlotWidget::SetShopSlot()
{
	if (ItemCDO)
	{
		ItemIcon->SetBrushFromTexture(ItemCDO->ItemIcon);
		ItemName->SetText(ItemCDO->DisplayName);
		SetPrice(ItemCDO->BuyPrice);
	}
	else
	{
		ensure(false);
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMDefaultShopSlotWidget::SetUserSlot(const FMItemHandle& ItemHandle)
{
	APlayerController* PlayerController = GetOwningPlayer();
	UPMInventoryManagerComponent* InvenManager = PlayerController ? PlayerController->FindComponentByClass<UPMInventoryManagerComponent>() : nullptr;
	if (ItemCDO && InvenManager)
	{
		ItemIcon->SetBrushFromTexture(ItemCDO->ItemIcon);
		ItemName->SetText(ItemCDO->DisplayName);
		SetPrice(ItemCDO->SellPrice);
		FPMInventoryEntry*  Entry = InvenManager->FindEntry(ItemHandle);
		if (Entry && Entry->GetItemQuentity() > 0)
		{
			OnChangeHandle = InvenManager->AddDelegateOnChangeItemQuentity(Entry->ItemUid, FOnChangeItemQuentity::FDelegate::CreateUObject(this, &UMDefaultShopSlotWidget::Callback_OnChangeItem));
			OnRemoveHandle = InvenManager->Delegate_OnRemoveItem.AddUObject(this, &UMDefaultShopSlotWidget::Callback_OnRemoveItem);
			ItemCount->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			ItemCount->SetText(FText::AsNumber(Entry->GetItemQuentity()));
		}
	}
	else
	{
		ensure(false);
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMDefaultShopSlotWidget::SetPrice(int32 NewPrice)
{
	Price->SetText(FText::AsNumber(NewPrice));
	NotEnoughPrice->SetText(FText::AsNumber(NewPrice));
}

void UMDefaultShopSlotWidget::OnClickItem_Buy()
{
	APlayerController* PlayerController = GetOwningPlayer();
	UMTradeComponentBase* PlayerTradeComponent = PlayerController ? PlayerController->FindComponentByClass<UMTradeComponentBase>() : nullptr;
	AActor* ShopNpc = Cast<AActor>(WidgetInfo.WidgetOwnerActor);
	if (!ShopNpc || !PlayerTradeComponent)
	{
		ensure(false);
		return ;
	}

	if (PlayerWalletComp.IsValid() && PlayerWalletComp->GetGold() < ItemCDO->BuyPrice)
	{
		MCHAE_LOG("Not enough money");
		return;
	}

	FMTradeRequest Request;
	Request.RequestType = EMRequestType::Trade;
	Request.RequiredGold = ItemCDO->BuyPrice;
	Request.GrantItems.Add({RowId, 1});

	PlayerTradeComponent->Server_OnRequestSimpleTrading(ShopNpc, Request);
}

void UMDefaultShopSlotWidget::OnClickItem_Sell()
{
	APlayerController* PlayerController = GetOwningPlayer();
	UMTradeComponentBase* PlayerTradeComponent = PlayerController ? PlayerController->FindComponentByClass<UMTradeComponentBase>() : nullptr;
	AActor* ShopNpc = Cast<AActor>(WidgetInfo.WidgetOwnerActor);

	if (!ShopNpc || !PlayerTradeComponent)
	{
		ensure(false);
		return;
	}

	FMTradeRequest Request;
	Request.RequestType = EMRequestType::Trade;
	Request.RequiredItems.Add({ RowId, 1, CachedItemHandle});
	Request.GrantGold = ItemCDO->SellPrice;

	PlayerTradeComponent->Server_OnRequestSimpleTrading(ShopNpc, Request);
}

void UMDefaultShopSlotWidget::Callback_OnChangeItem(const FMItemResponse& ItemRespons)
{
	ItemCount->SetText(FText::AsNumber(ItemRespons.ItemQuentity));
}

void UMDefaultShopSlotWidget::Callback_OnRemoveItem(const FMItemResponse& ItemRespons)
{
	if (Type == EMShopDetailType::Shop)
	{

	}
	else if (CachedItemHandle == ItemRespons.ItemRequest.ItemHandle)
	{
		RemoveFromParent();
	}
}
