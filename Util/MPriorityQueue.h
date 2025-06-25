#include "CoreMinimal.h"

template<typename T>
struct MPriorityQueueNode
{
	bool IsValid() const { return Keyid != INDEX_NONE; }
	bool operator>(const MPriorityQueueNode<T>& Other) const {
		return Keyid > Other.Keyid;
	}
	bool operator<(const MPriorityQueueNode<T>& Other) const {
		return Keyid < Other.Keyid;
	}
	bool operator==(const MPriorityQueueNode<T>& Other) const {
		return Keyid == Other.Keyid;
	}

	int ItemType = INDEX_NONE;
	int Keyid = INDEX_NONE;
	T* Data = nullptr;
};

/**
 *
 */
 template<typename T>
class MPriorityQueue
{

/*
* Member Functions
*/
public:
	MPriorityQueue() {
		Heap.Add({});
	}

	bool	IsEmpty() const { return Num() == 0; }
	int	    Num() const { return (int)Heap.Num() - 1; }

	bool	Peek(MPriorityQueueNode<T>& OutNode)
	{
		if (IsEmpty())
		{
			return false;
		}

		OutNode = Heap[1];
		return true;
	}

	// Leap에서 부모로 재귀적으로 올려보낸다
	void	Push(MPriorityQueueNode<T>&& InNode)
	{
		Heap.Add(InNode);

		PushHeap(Num(), InNode);
	}
	void	Push(const MPriorityQueueNode<T>& InNode)
	{
		Heap.Add(InNode);

		PushHeap(Num(), InNode);
	}
	void	Push_Unique(MPriorityQueueNode<T>&& InNode)
	{
		Heap.AddUnique(InNode);

		PushHeap(Num(), InNode);
	}
	void	Push_Unique(const MPriorityQueueNode<T>& InNode)
	{
		Heap.AddUnique(InNode);

		PushHeap(Num(), InNode);
	}
	// Leap를 Root로 지정 후 내려간다
	bool	Pop(MPriorityQueueNode<T>& OutNode)
	{
		if (IsEmpty())
		{
			return false;
		}

		OutNode = Heap[1];
		Swap(Heap[1], Heap[Num()]);
		Heap.Pop();
		PopHeap(1);

		return true;
	}

private:
	void    Swap(MPriorityQueueNode<T>& First, MPriorityQueueNode<T>& Second)
	{
		MPriorityQueueNode Temp = First;
		First = Second;
		Second = Temp;
	}
	void	PushHeap(const int NodeIndex, const MPriorityQueueNode<T>& NewNode)
	{
		int ParentIndex = GetParentIndex(NodeIndex);

		if (ParentIndex > 0 && Heap[ParentIndex] < NewNode)
		{
			Heap[NodeIndex] = Heap[ParentIndex];
			PushHeap(ParentIndex, NewNode);
		}
		else
		{
			Heap[NodeIndex] = NewNode;
		}
	}
	void    PopHeap(const int NodeIndex)
	{
		int RightChildIndex = GetRightChildIndex(NodeIndex);
		int LeftChildIndex = GetLeftChildIndex(NodeIndex);

		if (RightChildIndex > Num() || LeftChildIndex > Num())
		{
			return;
		}

		int LargestIndex = NodeIndex;

		if (Heap[LeftChildIndex] > Heap[LargestIndex])
		{
			LargestIndex = LeftChildIndex;
		}

		if (Heap[RightChildIndex] > Heap[LargestIndex])
		{
			LargestIndex = RightChildIndex;
		}

		if (LargestIndex != NodeIndex)
		{
			Swap(Heap[NodeIndex], Heap[LargestIndex]);
			PopHeap(LargestIndex);
		}
	}

	int		GetParentIndex(const int NodeIndex) const
	{
		if (NodeIndex * 0.5 > 0)
		{
			return (int)(NodeIndex * 0.5);
		}

		return 0;
	}
	int		GetLeftChildIndex(const int NodeIndex) const
	{
		return NodeIndex * 2;
	}
	int		GetRightChildIndex(const int NodeIndex) const
	{
		return NodeIndex * 2 + 1;
	}

/*
* Member Variables
*/

public:
// private:
/*
* index 0번은 비워둠
* 부모인덱스 * 2 = 왼쪽 자식 인덱스
* 부모 인덱스 * 2 + 1 = 오른쪽 자식 인덱스
* 자식 인덱스 / 2 = 부모인덱스
*/
	TArray<MPriorityQueueNode<T>> Heap;

	int LeapIndex = -1;
};
