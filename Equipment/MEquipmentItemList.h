#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Inventory/MInventoryTypes.h"

#include "MEquipmentItemList.generated.h"

class UMEquipmentItemInstance;
class UMEquipmentItemDefinition;
class UPMEquipmentManagerComponent;

USTRUCT(BlueprintType)
struct FMEquipmentItemEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	bool IsValid() const;

	EMItemType		GetItemType() const;
	FMItemHandle	GetItemHandle() const;
	int32			GetItemRowId() const;
	int32			GetItemQuentity() const;

	TSubclassOf<UMEquipmentItemDefinition> GetItemDefinition() const;

	UPROPERTY(BlueprintReadOnly)
	int32 ItemUid = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMEquipmentItemInstance> Instance = nullptr;
};

// 아이템들을 관리하는 객체
USTRUCT(BlueprintType)
struct FMEquipmentItemList : public FFastArraySerializer
{
	GENERATED_BODY()

	FMEquipmentItemList();
	FMEquipmentItemList(UPMEquipmentManagerComponent* InOwnerComponent);

/*
* Network
*/
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FMEquipmentItemEntry, FMEquipmentItemList>(Entries, DeltaParms, *this);
	}

/*
* Member Function
*/
public:
	int32 ChangeItemQuantity(const FMItemHandle& ItemHandle, int32 ChangeNum);

	FMItemHandle AddEntry(TSubclassOf<UMEquipmentItemDefinition> ItemDef, int32 ItemQuentity);
	void RemoveEntry(const FMItemHandle& ItemHandle);

	FMEquipmentItemEntry*	FindEntry(const FMItemHandle& ItemHandle);
	FMEquipmentItemEntry*	FindEntry(TSubclassOf<UMEquipmentItemDefinition> ItemDef);

protected:
	FMEquipmentItemEntry*	MakeEntry(TSubclassOf<UMEquipmentItemDefinition> ItemDef, int32 ItemQuentity);
	FMItemHandle			AddEntry_Impl(FMEquipmentItemEntry& Entry);

/*
* Member Variables
*/
public:
	// 실제 아이템의 Instance를 보유하고있는 배열
	UPROPERTY()
	TArray<FMEquipmentItemEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UPMEquipmentManagerComponent> OwnerComponent;

	UPROPERTY()
	EMItemType OwnedItemType = EMItemType::None;
};

template<>
struct TStructOpsTypeTraits<FMEquipmentItemList> : public TStructOpsTypeTraitsBase2<FMEquipmentItemList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};
