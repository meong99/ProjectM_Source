#include "MEquipmentItemList.h"
#include "Engine/Engine.h"
#include "Components/ActorComponent.h"
#include "PMGameplayTags.h"
#include "Item/Equipment/MEquipmentItemDefinition.h"
#include "PMEquipmentManagerComponent.h"
#include "Item/Equipment/MEquipmentItemInstance.h"

/*
* FMEquipmentItemEntry -------------------------------
*/
bool FMEquipmentItemEntry::IsValid() const
{
	return ItemUid != INDEX_NONE && Instance != nullptr;
}

EMItemType FMEquipmentItemEntry::GetItemType() const
{
	if (Instance)
	{
		return Instance->GetItemType();
	}

	MCHAE_WARNING("Equipment Item instance is not valid.");

	return EMItemType::None;
}

FMItemHandle FMEquipmentItemEntry::GetItemHandle() const
{
	if (Instance)
	{
		return Instance->ItemHandle;
	}

	MCHAE_WARNING("Equipment Item instance is not valid.");

	return FMItemHandle{};
}

int32 FMEquipmentItemEntry::GetItemRowId() const
{
	if (Instance)
	{
		return Instance->ItemRowId;
	}

	return 0;
}

int32 FMEquipmentItemEntry::GetItemQuentity() const
{
	if (Instance)
	{
		return Instance->GetItemQuentity();
	}

	return 0;
}

TSubclassOf<UMEquipmentItemDefinition> FMEquipmentItemEntry::GetItemDefinition() const
{
	if (Instance)
	{
		return Instance->ItemDef.Get();
	}

	return nullptr;
}

/*
* FPMInventoryList -------------------------------
*/
FMEquipmentItemList::FMEquipmentItemList()
{
	if (GEngine && GEngine->GetWorld() && GEngine->GetWorld()->GetGameInstance()
		&& IsValid(OwnerComponent) == false)
	{
		MCHAE_FETAL("OwnerComponent must be set!!");
	}
}

FMEquipmentItemList::FMEquipmentItemList(UPMEquipmentManagerComponent* InOwnerComponent)
	: OwnerComponent(InOwnerComponent)
{
	if (IsValid(OwnerComponent) == false)
	{
		MCHAE_FETAL("OwnerComponent must be set!!");
	}
}

void FMEquipmentItemList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		if (Entries.IsValidIndex(Index) && Entries[Index].Instance)
		{
			UMEquipmentItemInstance* Instance = Entries[Index].Instance;
		}
	}
}

void FMEquipmentItemList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		if (Entries.IsValidIndex(Index) && Entries[Index].Instance)
		{
			UMEquipmentItemInstance* Instance = Entries[Index].Instance;
		}
	}
}

void FMEquipmentItemList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		if (Entries.IsValidIndex(Index) && Entries[Index].Instance)
		{
		}
	}
}

int32 FMEquipmentItemList::ChangeItemQuantity(const FMItemHandle& ItemHandle, int32 ChangeNum)
{
	FMEquipmentItemEntry* Entry = FindEntry(ItemHandle);

	if (Entry == nullptr)
	{
		MCHAE_WARNING("Can't found item.");
		return -1;
	}

	if (Entry->Instance == nullptr)
	{
		MCHAE_WARNING("It's invalid Entry! Item intance is not created or removed in entry.");
		return -1;
	}

	if (ChangeNum > 0)
	{
		MarkItemDirty(*Entry);
		return Entry->Instance->AddStatTagStack(FPMGameplayTags::Get().Item_Quentity, ChangeNum);
	}
	else if (ChangeNum < 0)
	{
		MarkItemDirty(*Entry);
		return Entry->Instance->RemoveStatTagStack(FPMGameplayTags::Get().Item_Quentity, ChangeNum * -1);
	}

	MCHAE_WARNING("StackCount is zero");
	return -1;
}

FMItemHandle FMEquipmentItemList::AddEntry(TSubclassOf<UMEquipmentItemDefinition> ItemDef, int32 ItemQuentity)
{
	if (!ItemDef || !OwnerComponent)
	{
		MCHAE_WARNING("ItemDefinition is not valid");
		return {};
	}

	AActor* OwningActor = OwnerComponent->GetOwner();
	if (!OwningActor->HasAuthority())
	{
		MCHAE_WARNING("AddEntry only allowed on authority. You called on client");
		return {};
	}


	FMEquipmentItemEntry* NewEntry = MakeEntry(ItemDef, ItemQuentity);
	AddEntry_Impl(*NewEntry);

	return NewEntry->GetItemHandle();
}

FMItemHandle FMEquipmentItemList::AddEntry_Impl(FMEquipmentItemEntry& Entry)
{
	FMItemHandle NewHandle = FMItemHandle{};

	if (!Entry.Instance)
	{
		ensure(false);
		MCHAE_WARNING("ItemInstance is not valid!");

		return NewHandle;
	}

	static int32 ItemUid = 0;

	NewHandle.ItemUid = ItemUid;
	NewHandle.ItemType = Entry.Instance->GetItemType();

	Entry.ItemUid = ItemUid;
	Entry.Instance->ItemHandle = NewHandle;
	Entry.Instance->ItemResponse.ItemRequest.ItemHandle = NewHandle;

	ItemUid++;

	MarkItemDirty(Entry);

	return NewHandle;
}

void FMEquipmentItemList::RemoveEntry(const FMItemHandle& ItemHandle)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FMEquipmentItemEntry& Entry = *EntryIt;
		if (Entry.ItemUid == ItemHandle.ItemUid)
		{
			EntryIt.RemoveCurrent();
			break;
		}
	}
	MarkArrayDirty();
}

FMEquipmentItemEntry* FMEquipmentItemList::MakeEntry(TSubclassOf<UMEquipmentItemDefinition> ItemDef, int32 ItemQuentity)
{
	AActor* OwningActor = OwnerComponent->GetOwner();
	const UMEquipmentItemDefinition* ItemDefCDO = GetDefault<UMEquipmentItemDefinition>(ItemDef);
	TSubclassOf<UMEquipmentItemInstance> InstanceType = ItemDefCDO->EquipmentInstanceType;
	if (!InstanceType)
	{
		InstanceType = UMEquipmentItemInstance::StaticClass();
	}

	FMEquipmentItemEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<UMEquipmentItemInstance>(OwningActor, InstanceType);
	NewEntry.Instance->ItemDef = ItemDef;
	NewEntry.Instance->ItemRowId = ItemDefCDO->RowId;

	for (const UPMInventoryItemFragment* Fragment : GetDefault<UMEquipmentItemDefinition>(ItemDef)->GetFragments())
	{
		if (Fragment)
		{
			Fragment->OnInstanceCreated(NewEntry.Instance);
		}
	}

	NewEntry.Instance->OnInstanceCreated();

	if (ItemQuentity > 1)
	{
		NewEntry.Instance->AddStatTagStack(FPMGameplayTags::Get().Item_Quentity, ItemQuentity - 1);
	}

	return &NewEntry;
}

FMEquipmentItemEntry* FMEquipmentItemList::FindEntry(const FMItemHandle& ItemHandle)
{
#pragma TODO("ItemList를 Tree로 구현해서 사용해야한다.")
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FMEquipmentItemEntry& Entry = *EntryIt;
		if (Entry.ItemUid == ItemHandle.ItemUid)
		{
			return &Entry;
		}
	}

	return nullptr;
}

FMEquipmentItemEntry* FMEquipmentItemList::FindEntry(TSubclassOf<UMEquipmentItemDefinition> ItemDef)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FMEquipmentItemEntry& Entry = *EntryIt;
		if (Entry.GetItemDefinition() == ItemDef)
		{
			return &Entry;
		}
	}

	return nullptr;
}
