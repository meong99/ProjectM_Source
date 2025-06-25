#pragma once

#include "CoreMinimal.h"

// 기본이 최대힙(Less)이며 최소힙을 사용할 땐 TGreater를 사용한다.
template <typename T, typename Compare = TLess<T>>
class TMHeap
{
private:
	TArray<T>	Data;
	TSet<T>		DataCheck;
	Compare		Comp;
	
public:
	TMHeap()
	{
		Heapify();
	}

	void Push(T Value)
	{
		if (!DataCheck.Find(Value))
		{
			Data.HeapPush(Value, Comp);
			DataCheck.Add(Value);
		}
	}

	void Push(T&& Value)
	{
		if (!DataCheck.Find(Value))
		{
			Data.HeapPush(MoveTemp(Value), Comp);
			DataCheck.Add(Value);
		}
	}

	T Pop()
	{
		T Item;
		Data.HeapPop(Item, Comp);
		DataCheck.Remove(Item);

		return Item;
	}

	T Top() const
	{
		if (Data.IsEmpty())
		{
			MCHAE_WARNING("Data is empty");
			return;
		}

		return Data.HeapTop();
	}

	bool IsEmpty() const
	{
		return Data.IsEmpty();
	}

	int32 Num() const
	{
		return Data.Num();
	}

	void RemoveElement(const T& Element)
	{
		bool bIsRemoved = false;

		for (int32 i = 0; i < Data.Num(); i++)
		{
			if (Data[i] == Element)
			{
				Data.RemoveAt(i);
				DataCheck.Remove(Element);
				bIsRemoved = true;
				break;
			}
		}

		if (bIsRemoved)
		{
			Data.Heapify();
		}
	}

	void Heapify()
	{
		Data.Heapify(Comp);
	}
};