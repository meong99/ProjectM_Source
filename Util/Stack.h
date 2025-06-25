#pragma once

#include "CoreMinimal.h"

template <typename T>
class TStack
{
private:
	TArray<T>	Data;
	
public:
	TStack()
	{
	}

	void Push(T Value)
	{
		Data.Push(Value);
	}

	void Push(T&& Value)
	{
		Data.Push(MoveTemp(Value));
	}

	T Pop()
	{
		if (Data.IsEmpty())
		{
			ensure(false);
		}
		return Data.Pop();
	}

	T Top() const
	{
		if (Data.IsEmpty())
		{
			ensure(false);
		}
		return Data.Top();
	}

	bool IsEmpty() const
	{
		return Data.IsEmpty();
	}

	int32 Num() const
	{
		return Data.Num();
	}
};