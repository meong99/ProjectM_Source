#include "PMGameplayTagStack.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(PMGameplayTagStack)

int32 FPMGameplayTagStackContainer::AddStack(FGameplayTag Tag, int32 StackCount)
{
	if (!Tag.IsValid())
	{
		MCHAE_WARNING("StackTag is not valid");
		return -1;
	}

	if (StackCount > 0)
	{
		// linear search...
		// - we can't say this is performant, but my guess is that the number of Stacks should be less than dozens
		for (FPMGameplayTagStack& Stack : Stacks)
		{
			if (Stack.Tag == Tag)
			{
				const int32 NewCount = Stack.StackCount + StackCount;
				Stack.StackCount = NewCount;
				TagToCountMap[Tag] = NewCount;
				MarkItemDirty(Stack);
				return NewCount;
			}
		}

		FPMGameplayTagStack& NewStack = Stacks.Emplace_GetRef(Tag, StackCount);

		MarkItemDirty(NewStack);
		// if we reach to this line of code, the initial StackCount is 0
		TagToCountMap.Add(Tag, StackCount);

		return StackCount;
	}

	MCHAE_WARNING("StackCount is zero");
	return -1;
}

int32 FPMGameplayTagStackContainer::RemoveStack(FGameplayTag Tag, int32 StackCount)
{
	if (!Tag.IsValid())
	{
		MCHAE_WARNING("StackTag is not valid");
		return -1;
	}

	if (StackCount > 0)
	{
		// we use Iterator pattern to search, cuz it is more convenient to erase elements while iterating
		for (auto It = Stacks.CreateIterator(); It; ++It)
		{
			FPMGameplayTagStack& Stack = *It;
			if (Stack.Tag == Tag)
			{
				// we reach to zero (apparently less than zero)
				if (Stack.StackCount <= StackCount)
				{
					// THIS IS THE WAY TO DELETE ELEMENT WHILE WE ITERATE
					It.RemoveCurrent();
					TagToCountMap.Remove(Tag);
					MarkArrayDirty();

					return 0;
				}

				const int32 NewCount = Stack.StackCount - StackCount;
				Stack.StackCount = NewCount;
				TagToCountMap[Tag] = NewCount;
				MarkItemDirty(Stack);

				return NewCount;
			}
		}
	}

	MCHAE_WARNING("StackCount is zero");
	return -1;
}

void FPMGameplayTagStackContainer::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		const FGameplayTag Tag = Stacks[Index].Tag;
		TagToCountMap.Remove(Tag);
	}
}

void FPMGameplayTagStackContainer::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		const FPMGameplayTagStack& Stack = Stacks[Index];
		TagToCountMap.Add(Stack.Tag, Stack.StackCount);
	}
}

void FPMGameplayTagStackContainer::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		const FPMGameplayTagStack& Stack = Stacks[Index];
		TagToCountMap[Stack.Tag] = Stack.StackCount;
	}
}
