#pragma once

#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "PMGameplayTagStack.generated.h"

/**
 * Represents one stack of a gameplay tag (tag + count)
 * : for example, Ammo is representative example for GameplayTagStack
 */
USTRUCT(BlueprintType)
struct FPMGameplayTagStack : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FPMGameplayTagStack() {}
	FPMGameplayTagStack(FGameplayTag InTag, int32 InStackCount)
		: Tag(InTag)
		, StackCount(InStackCount)
	{}

	UPROPERTY()
	FGameplayTag Tag;

	UPROPERTY()
	int32 StackCount = 0;
};

/** container of PMGameplayTagStack */
USTRUCT(BlueprintType)
struct FPMGameplayTagStackContainer : public FFastArraySerializer
{
	GENERATED_BODY()

	FPMGameplayTagStackContainer() {}

	/** add/remove stack count by gameplay-tag */
	int32 AddStack(FGameplayTag Tag, int32 StackCount);
	int32 RemoveStack(FGameplayTag Tag, int32 StackCount);

	/** get the count by the gameplay tag */
	int32 GetStackCount(FGameplayTag Tag) const
	{
		return TagToCountMap.FindRef(Tag);
	}

	/** whether gameplay tag exists in PMGameplayTagStackContainer */
	bool ContainsTag(FGameplayTag Tag) const
	{
		return TagToCountMap.Contains(Tag);
	}

	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FPMGameplayTagStack, FPMGameplayTagStackContainer>(Stacks, DeltaParms, *this);
	}

	/** a list of gameplay tag stacks */
	UPROPERTY()
	TArray<FPMGameplayTagStack> Stacks;

	/**
	 * LUT(Look-up table) to accelerate gameplay tag stack to query [GameplayTag -> Count]
	 * - we also use this LUT to find existance for corresponding gameplay tag
	 */
	TMap<FGameplayTag, int32> TagToCountMap;
};

template<>
struct TStructOpsTypeTraits<FPMGameplayTagStackContainer> : public TStructOpsTypeTraitsBase2<FPMGameplayTagStackContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};
