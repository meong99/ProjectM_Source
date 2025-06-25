#include "MPlayerTradeComponent.h"
#include "Inventory/PMInventoryManagerComponent.h"
#include "Character/Components/MWalletComponent.h"

UMPlayerTradeComponent::UMPlayerTradeComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMPlayerTradeComponent::Server_OnRequestSimpleDataGrant_Implementation(AActor* Requestor, const FMTradeRequest& Request)
{
	if (GetOwnerRole() != ENetRole::ROLE_Authority)
	{
		ensure(false);
		return ;
	}

	AActor* Owner = GetOwner();
	UPMInventoryManagerComponent* InventoryManager = Owner->FindComponentByClass<UPMInventoryManagerComponent>();
	UMWalletComponent* WalletComponent = Owner->FindComponentByClass<UMWalletComponent>();

	if (!WalletComponent || !InventoryManager)
	{
		ensure(false);
		return;
	}

	WalletComponent->AddGold(Request.GrantGold);
	for (int32 i = 0; i < Request.GrantItems.Num(); i++)
	{
		int32 GrantItemNum = Request.GrantItems[i].ItemCount;

		FMItemRequest ItemRequest;
		ItemRequest.SetItemRequest(EMItemRequestType::AddItem, Request.GrantItems[i].ItemRowId, GrantItemNum);

		InventoryManager->RequestItemChange(ItemRequest);
	}
}

void UMPlayerTradeComponent::Server_OnRequestSimpleTrading_Implementation(AActor* Requestor, const FMTradeRequest& TradeRequest)
{
	if (GetOwnerRole() != ENetRole::ROLE_Authority)
	{
		ensure(false);
		return;
	}

	AActor* Owner = GetOwner();
	UPMInventoryManagerComponent* InventoryManager = Owner ? Owner->FindComponentByClass<UPMInventoryManagerComponent>() : nullptr;
	UMWalletComponent* WalletComponent = Owner ? Owner->FindComponentByClass<UMWalletComponent>() : nullptr;
	if (!InventoryManager || !WalletComponent)
	{
		ensure(false);
		return;
	}

	if (TradeRequest.RequiredGold > 0)
	{
		if (TradeRequest.RequiredGold > WalletComponent->GetGold())
		{
			return;
		}

		WalletComponent->SubtractGold(TradeRequest.RequiredGold);
	}

	if (TradeRequest.RequiredItems.Num() > 0)
	{
		for (int32 i = 0; i < TradeRequest.RequiredItems.Num(); i++)
		{
			int32 RequirementNum = TradeRequest.RequiredItems[i].ItemCount;
			if (RequirementNum > InventoryManager->GetItemQuantity(TradeRequest.RequiredItems[i].ItemRowId))
			{
				return;
			}
		}

		for (int32 i = 0; i < TradeRequest.RequiredItems.Num(); i++)
		{
			int32 RequirementNum = TradeRequest.RequiredItems[i].ItemCount;
			FMItemRequest ItemRequest;
			ItemRequest.SetItemRequest(EMItemRequestType::RemoveItem, TradeRequest.RequiredItems[i].ItemRowId, -RequirementNum, TradeRequest.RequiredItems[i].Handle);
			InventoryManager->RequestItemChange(ItemRequest);
		}
	}

	WalletComponent->AddGold(TradeRequest.GrantGold);
	for (int32 i = 0; i < TradeRequest.GrantItems.Num(); i++)
	{
		int32 GrantItemNum = TradeRequest.GrantItems[i].ItemCount;

		FMItemRequest ItemRequest;
		ItemRequest.SetItemRequest(EMItemRequestType::AddItem, TradeRequest.GrantItems[i].ItemRowId, GrantItemNum);
		InventoryManager->RequestItemChange(ItemRequest);
	}
}
