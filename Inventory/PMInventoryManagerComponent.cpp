#include "PMInventoryManagerComponent.h"
#include "PMInventoryItemInstance.h"
#include "PMInventoryItemDefinition.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"
#include "System/MDataTableManager.h"
#include "Engine/Engine.h"
#include "Engine/DataTable.h"
#include "Table/Item/MTable_ConsumableItem.h"
#include "Table/MTableAsset.h"
#include "Table/Item/MTable_EquipmentItem.h"
#include "PMGameplayTags.h"
#include "TimerManager.h"

/*
* UPMInventoryManagerComponent -------------------------------
*/
UPMInventoryManagerComponent::UPMInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryList(this)
	, ConsumableItemList(this)
	, MiscellaneousItemList(this)
{
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;

	InventoryList.OwnedItemType = EMItemType::Equipment;
	ConsumableItemList.OwnedItemType = EMItemType::Consumable;
	MiscellaneousItemList.OwnedItemType = EMItemType::Miscellaneous;
}

void UPMInventoryManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	InitInventory();
}

void UPMInventoryManagerComponent::CallOrRegister_OnInitInventory(FOnInitInventory::FDelegate&& Delegate)
{
	if (bIsInitInventory)
	{
		Delegate.Execute(InventoryList);
		Delegate.Execute(ConsumableItemList);
		Delegate.Execute(MiscellaneousItemList);
	}
	else
	{
		Delegate_OnInitInventory.Add(MoveTemp(Delegate));
	}
}

void UPMInventoryManagerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
	DOREPLIFETIME(ThisClass, ConsumableItemList);
	DOREPLIFETIME(ThisClass, MiscellaneousItemList);
}

bool UPMInventoryManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FPMInventoryEntry& Entry : InventoryList.Entries)
	{
		UPMInventoryItemInstance* Instance = Entry.Instance;

		if (Instance && IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	for (FPMInventoryEntry& Entry : ConsumableItemList.Entries)
	{
		UPMInventoryItemInstance* Instance = Entry.Instance;

		if (Instance && IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	for (FPMInventoryEntry& Entry : MiscellaneousItemList.Entries)
	{
		UPMInventoryItemInstance* Instance = Entry.Instance;

		if (Instance && IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void UPMInventoryManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

		// Register existing ULyraInventoryItemInstance
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FPMInventoryEntry& Entry : InventoryList.Entries)
		{
			UPMInventoryItemInstance* Instance = Entry.Instance;

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}

		for (const FPMInventoryEntry& Entry : ConsumableItemList.Entries)
		{
			UPMInventoryItemInstance* Instance = Entry.Instance;

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}

		for (const FPMInventoryEntry& Entry : MiscellaneousItemList.Entries)
		{
			UPMInventoryItemInstance* Instance = Entry.Instance;

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

FMItemHandle UPMInventoryManagerComponent::RequestItemChange(const FMItemRequest& ItemRequest)
{
	FMItemHandle Handle = FMItemHandle{};
	if (!HasAuthority())
	{
		ensure(false);
		return Handle;
	}

	UPMInventoryItemDefinition* ItemCDO = GetItemDefCDO(ItemRequest);
	if (!ItemCDO)
	{
		ensure(false);
		MCHAE_WARNING("ItemCDO is not valid");
		return Handle;
	}

	FPMInventoryItemList* ItemList = GetItemList(ItemCDO->ItemType);
	if (ItemList)
	{
		FPMInventoryEntry* Entry = ItemRequest.ItemHandle.IsValid() ? 
			ItemList->FindEntry(ItemRequest.ItemHandle) : ItemList->FindEntry(ItemCDO->GetClass());

		bool bCanChangeItemQuentity = ItemList->OwnedItemType != EMItemType::Equipment;
		bCanChangeItemQuentity &= (ItemRequest.RequestType == EMItemRequestType::AddItem || ItemRequest.RequestType == EMItemRequestType::RemoveItem);

		if (Entry && bCanChangeItemQuentity)
		{
			Handle = Entry->GetItemHandle();
			ChangeItemQuantity(Entry->Instance, ItemRequest);
		}
		else
		{
			Handle = AddItemDefinition_Impl(ItemCDO->GetClass(), *ItemList, ItemRequest);
		}
	}

	return Handle;
}

FMItemHandle UPMInventoryManagerComponent::AddItemDefinition_Impl(TSubclassOf<UPMInventoryItemDefinition> ItemDef, FPMInventoryItemList& ItemList, const FMItemRequest& ItemRequest)
{
	FMItemHandle Handle = FMItemHandle{};

	Handle = ItemList.AddEntry(ItemDef, ItemRequest.ItemQuentity);

	FPMInventoryEntry* Entry = ItemList.FindEntry(Handle);
	if (Entry == nullptr)
	{
		return FMItemHandle{};
	}

	UPMInventoryItemInstance* ItemInstance = Entry->Instance;

	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && ItemInstance)
	{
		AddReplicatedSubObject(ItemInstance);
	}

	ItemInstance->ItemResponse.SetItemResponse(ItemRequest, EMItemResponseType::TotallyNewItem, ItemInstance->GetItemQuentity(), ItemInstance->ItemHandle, ItemInstance);

	if (GetNetMode() == ENetMode::NM_Standalone)
	{
		Broadcast_OnItemIncreased(ItemInstance->ItemResponse);
	}

	return Handle;
}

FMItemHandle UPMInventoryManagerComponent::ReturnItem(UPMInventoryItemInstance* Instance)
{
	if (Instance)
	{
		FPMInventoryItemList* ItemList = GetItemList(Instance->GetItemType());

		if (ItemList)
		{
			Instance->ItemResponse.ResponsType = EMItemResponseType::TotallyNewItem;

			FMItemHandle Handle = ItemList->AddEntry(Instance);
			if (GetNetMode() == ENetMode::NM_Standalone)
			{
				Broadcast_OnItemIncreased(Instance->ItemResponse);
			}

			return Handle;
		}
	}

	return {};
}

UPMInventoryItemDefinition* UPMInventoryManagerComponent::GetItemDefCDO(const int32 ItemRowId)
{
	UPMInventoryItemDefinition* ItemCDO = nullptr;

	UMDataTableManager* TableManager = GEngine->GetEngineSubsystem<UMDataTableManager>();
	if (TableManager)
	{
		ItemCDO = UMDataTableManager::GetDefinitionObject<UPMInventoryItemDefinition>(this, ItemRowId);
	}

	return ItemCDO;
}

UPMInventoryItemDefinition* UPMInventoryManagerComponent::GetItemDefCDO(const TSubclassOf<UPMInventoryItemDefinition>& ItemDef)
{
	if (ItemDef)
	{
		return ItemDef->GetDefaultObject<UPMInventoryItemDefinition>();
	}

	return nullptr;
}

UPMInventoryItemDefinition* UPMInventoryManagerComponent::GetItemDefCDO(const FMItemRequest& ItemRequest)
{
	return GetItemDefCDO(ItemRequest.ItemRowId);
}

UPMInventoryItemInstance* UPMInventoryManagerComponent::FindItemInstance(const FMItemHandle& ItemHandle)
{
	FPMInventoryItemList* ItemList = GetItemList(ItemHandle.ItemType);
	if (ItemList)
	{
		FPMInventoryEntry* Entry = ItemList->FindEntry(ItemHandle);
		if (Entry)
		{
			return Entry->Instance;
		}
	}

	return nullptr;
}

FPMInventoryEntry* UPMInventoryManagerComponent::FindEntry(const FMItemHandle& ItemHandle)
{
	FPMInventoryItemList* ItemList = GetItemList(ItemHandle.ItemType);
	if (ItemList)
	{
		return ItemList->FindEntry(ItemHandle);
	}

	return nullptr;
}

FPMInventoryEntry* UPMInventoryManagerComponent::FindEntry(TSubclassOf<UPMInventoryItemDefinition> ItemDef)
{
	UPMInventoryItemDefinition* CDO = ItemDef->GetDefaultObject<UPMInventoryItemDefinition>();
	if (CDO)
	{
		FPMInventoryItemList* ItemList = GetItemList(CDO->ItemType);
		if (ItemList)
		{
			return ItemList->FindEntry(ItemDef);
		}
	}

	return nullptr;
}

int32 UPMInventoryManagerComponent::GetItemQuantity(const int32 ItemRowId)
{
	UMDataTableManager* TableManager = GEngine->GetEngineSubsystem<UMDataTableManager>();
	UPMInventoryItemDefinition* ItemDef = TableManager->GetItemDefinition(ItemRowId);
	if (!ItemDef)
	{
		ensure(false);
		return 0;
	}

	FPMInventoryItemList* ItemList = GetItemList(ItemDef->ItemType);
	if (!ItemList)
	{
		ensure(false);
		return 0;
	}

	int32 Result = 0;
	for (const FPMInventoryEntry& Entry : ItemList->Entries)
	{
		if (Entry.GetItemRowId() == ItemRowId && Entry.Instance)
		{
			Result += Entry.Instance->GetStatTagStackCount(FPMGameplayTags::Get().Item_Quentity);
		}
	}

	return Result;
}

int32 UPMInventoryManagerComponent::ChangeItemQuantity(UPMInventoryItemInstance* ItemInstance, const FMItemRequest& ItemRequest)
{
	if (!ItemInstance)
	{
		ensure(false);
		MCHAE_WARNING("Item Instance is null!");
		return 0;
	}

	FPMInventoryItemList* ItemList = GetItemList(ItemInstance->ItemHandle.ItemType);
	if (ItemList)
	{
		int32 ItemQuentity = 0;
		if (ItemRequest.RequestType == EMItemRequestType::AddItem)
		{
			ItemQuentity = ItemRequest.ItemQuentity > 0 ? ItemRequest.ItemQuentity : -ItemRequest.ItemQuentity;
		}
		else if (ItemRequest.RequestType == EMItemRequestType::RemoveItem)
		{
			ItemQuentity = ItemRequest.ItemQuentity < 0 ? ItemRequest.ItemQuentity : -ItemRequest.ItemQuentity;
		}

		int32 CurrentItemQuentity = ItemList->ChangeItemQuantity(ItemInstance->ItemHandle, ItemQuentity);

		if (CurrentItemQuentity == 0)
		{
			RemoveItem(ItemInstance, ItemRequest);
		}
		else
		{
			ItemInstance->ItemResponse.SetItemResponse(ItemRequest, EMItemResponseType::ChangeItemQuentity, CurrentItemQuentity, ItemInstance->ItemHandle, ItemInstance);

			if (GetNetMode() == ENetMode::NM_Standalone)
			{
				Broadcast_OnChangeItemQuentity(ItemInstance->ItemResponse);
				if (ItemRequest.RequestType == EMItemRequestType::AddItem)
				{
					Broadcast_OnItemIncreased(ItemInstance->ItemResponse);
				}
				else if (ItemRequest.RequestType == EMItemRequestType::RemoveItem)
				{
					Broadcast_OnItemDecreased(ItemInstance->ItemResponse);
				}
			}
		}

		return CurrentItemQuentity;
	}

	return 0;
}

FDelegateHandle UPMInventoryManagerComponent::AddDelegateOnChangeItemQuentity(const int32 ItemUid, FOnChangeItemQuentity::FDelegate&& Delegate)
{
	FOnChangeItemQuentity& InventoryDelegates = DelegateMap_OnChangeItemQuentity.FindOrAdd(ItemUid);

	FDelegateHandle DelegateHandle = InventoryDelegates.Add(Delegate);

	return DelegateHandle;
}

void UPMInventoryManagerComponent::RemoveDelegateOnChangeItemQuentity(const int32 ItemUid, const FDelegateHandle& DelegateHandle)
{
	FOnChangeItemQuentity* InventoryDelegates = DelegateMap_OnChangeItemQuentity.Find(ItemUid);

	if (InventoryDelegates)
	{
		InventoryDelegates->Remove(DelegateHandle);
		
		if (InventoryDelegates->IsBound() == false)
		{
			DelegateMap_OnChangeItemQuentity.Remove(ItemUid);
		}
	}
}

void UPMInventoryManagerComponent::Server_UseItem_Implementation(const FMItemHandle& ItemHandle)
{
	UPMInventoryItemInstance* ItemInstance = FindItemInstance(ItemHandle);
	if (ItemInstance)
	{
		ItemInstance->ActivateItem();
	}
}

void UPMInventoryManagerComponent::InitInventory()
{
	bIsInitInventory = true;
	Delegate_OnInitInventory.Broadcast(InventoryList);
	Delegate_OnInitInventory.Broadcast(ConsumableItemList);
	Delegate_OnInitInventory.Broadcast(MiscellaneousItemList);
	Delegate_OnInitInventory.Clear();
}

void UPMInventoryManagerComponent::RemoveItem(UPMInventoryItemInstance* ItemInstance, const FMItemRequest& ItemRequest)
{
	if (!ItemInstance)
	{
		ensure(false);
		return;
	}

	FPMInventoryItemList* ItemList = GetItemList(ItemInstance->ItemHandle.ItemType);
	if (ItemList)
	{
		if (GetNetMode() == ENetMode::NM_Standalone)
		{
			ItemInstance->ItemResponse.SetItemResponse(ItemRequest, EMItemResponseType::Removed, 0, ItemInstance->ItemHandle, ItemInstance);
			Broadcast_OnOnRemoveItem(ItemInstance->ItemResponse);
		}

		if (IsUsingRegisteredSubObjectList())
		{
			RemoveReplicatedSubObject(ItemInstance);
		}

		ItemList->RemoveEntry(ItemInstance->ItemHandle);
		DelegateMap_OnChangeItemQuentity.Remove(ItemInstance->ItemHandle.ItemUid);
	}
}

void UPMInventoryManagerComponent::Broadcast_OnItemIncreased(const FMItemResponse& ItemRespons)
{
	if (Delegate_OnRemoveItem.IsBound())
	{
		Delegate_OnItemIncreased.Broadcast(ItemRespons);
	}
}

void UPMInventoryManagerComponent::Broadcast_OnItemDecreased(const FMItemResponse& ItemRespons)
{
	if (Delegate_OnRemoveItem.IsBound())
	{
		Delegate_OnItemDecreased.Broadcast(ItemRespons);
	}
}

void UPMInventoryManagerComponent::Broadcast_OnOnRemoveItem(const FMItemResponse& ItemRespons)
{
	if (Delegate_OnRemoveItem.IsBound())
	{
		Delegate_OnRemoveItem.Broadcast(ItemRespons);
	}
}

void UPMInventoryManagerComponent::Broadcast_OnChangeItemQuentity(const FMItemResponse& ItemRespons)
{
	FOnChangeItemQuentity* InventoryDelegates = DelegateMap_OnChangeItemQuentity.Find(ItemRespons.ItemRequest.ItemHandle.ItemUid);
	if (InventoryDelegates && InventoryDelegates->IsBound())
	{
		InventoryDelegates->Broadcast(ItemRespons);
	}
}

FPMInventoryItemList* UPMInventoryManagerComponent::GetItemList(const EMItemType ItemType)
{
	switch (ItemType)
	{
		case EMItemType::Equipment :
		{
			return &InventoryList;
		}
		case EMItemType::Consumable :
		{
			return &ConsumableItemList;
		}
		case EMItemType::Miscellaneous:
		{
			return &MiscellaneousItemList;
		}
		default:
		{
			MCHAE_WARNING("Can't Found ItemList");
			return nullptr;
		}
	}
}

void UPMInventoryManagerComponent::Debug_AddItem(int32 RowId, int32 ItemQuentity)
{
	if (GEngine)
	{
		UMDataTableManager* TableManager = GEngine->GetEngineSubsystem<UMDataTableManager>();
		if (TableManager)
		{
			const UDataTable* DataTable = TableManager->GetDataTable(RowId);
			if (DataTable)
			{
				int32 ElementId = UMDataTableManager::ChangeRowIdToElementId(RowId) - 1;
				const TArray<FName>& Names = DataTable->GetRowNames();
				if (Names.IsValidIndex(ElementId))
				{
					FMTable_TableBase* Item = DataTable->FindRow<FMTable_TableBase>(Names[ElementId], Names[ElementId].ToString());
					if (Item)
					{
						DebugServer_AddItem(Item->RowId, ItemQuentity);
					}
					else
					{
						MCHAE_LOG("Can't Found Item. RowId = %d", RowId);
					}
				}
			}
		}
	}
}

void UPMInventoryManagerComponent::DebugServer_AddItem_Implementation(int32 Rowid, int32 ItemQuentity)
{
	FMItemRequest Request;

	Request.SetItemRequest(EMItemRequestType::AddItem, Rowid, ItemQuentity);
	RequestItemChange(Request);
}
