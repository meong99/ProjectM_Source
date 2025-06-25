#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Inventory/MInventoryTypes.h"

#include "PMInventoryItemList.generated.h"

class UPMInventoryItemInstance;
class UPMInventoryItemDefinition;
class UPMInventoryManagerComponent;

USTRUCT(BlueprintType)
struct FPMInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	bool IsValid() const;

	EMItemType		GetItemType() const;
	FMItemHandle	GetItemHandle() const;
	int32			GetItemRowId() const;
	int32			GetItemQuentity() const;

	TSubclassOf<UPMInventoryItemDefinition> GetItemDefinition() const;

	UPROPERTY(BlueprintReadOnly)
	int32 ItemUid = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UPMInventoryItemInstance> Instance = nullptr;
};

// 아이템들을 관리하는 객체
USTRUCT(BlueprintType)
struct FPMInventoryItemList : public FFastArraySerializer
{
	GENERATED_BODY()

	FPMInventoryItemList();
	FPMInventoryItemList(UPMInventoryManagerComponent* InOwnerComponent);

/*
* Network
*/
 	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
 	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
 	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FPMInventoryEntry, FPMInventoryItemList>(Entries, DeltaParms, *this);
	}

/*
* Member Function
*/
public:
	int32 ChangeItemQuantity(const FMItemHandle& ItemHandle, int32 ChangeNum);

	FMItemHandle AddEntry(TSubclassOf<UPMInventoryItemDefinition> ItemDef, int32 ItemQuentity);
	FMItemHandle AddEntry(UPMInventoryItemInstance* Instance);
	void RemoveEntry(const FMItemHandle& ItemHandle);

	FPMInventoryEntry* FindEntry(const FMItemHandle& ItemHandle);
	FPMInventoryEntry* FindEntry(TSubclassOf<UPMInventoryItemDefinition> ItemDef);

protected:
	FPMInventoryEntry*	MakeEntry(TSubclassOf<UPMInventoryItemDefinition> ItemDef, int32 ItemQuentity);
	FPMInventoryEntry*	MakeEntry(UPMInventoryItemInstance* Instance);
	FMItemHandle		AddEntry_Impl(FPMInventoryEntry& Entry);

/*
* Member Variables
*/
public:
	// 실제 아이템의 Instance를 보유하고있는 배열
	UPROPERTY()
	TArray<FPMInventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UPMInventoryManagerComponent> OwnerComponent;

	UPROPERTY()
	EMItemType OwnedItemType = EMItemType::None;
};

template<>
struct TStructOpsTypeTraits<FPMInventoryItemList> : public TStructOpsTypeTraitsBase2<FPMInventoryItemList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};
