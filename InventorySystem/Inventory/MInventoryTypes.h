#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"

#include "MInventoryTypes.generated.h"

class UPMInventoryItemInstance;
class UPMInventoryItemDefinition;

UENUM(BlueprintType)
enum class EMItemType : uint8
{
	Equipment,
	Consumable,
	Miscellaneous,

	None,
};

USTRUCT(BlueprintType)
struct FMItemHandle 
{
	GENERATED_BODY()

	bool IsValid() const
	{
		return ItemUid != INDEX_NONE;
	}

	bool operator==(const FMItemHandle& Other) const
	{
		return ItemUid == Other.ItemUid;
	}

	void Reset()
	{
		ItemUid = INDEX_NONE;
		ItemType = EMItemType::None;
	}

	UPROPERTY(BlueprintReadWrite)
	int32 ItemUid = INDEX_NONE;

	UPROPERTY(BlueprintReadWrite)
	EMItemType ItemType = EMItemType::None;
};

UENUM(BlueprintType)
enum class EMItemRequestType : uint8
{
	None,

	// 초기화할 때 추가되는 아이템 1~n개
	InitItem,

	// 아이템 추가 1~n개
	AddItem,

	// 아이템 제거 1~n개
	RemoveItem,

	// 이미 인스턴스 존재하는 아이템이 인벤토리로 복귀. 1~n개. Request에 ItemInstance 추가 필수
	ReturnItemToInventory,
};

USTRUCT(BlueprintType)
struct FMItemRequest
{
	GENERATED_BODY()

	// 빼먹는 변수 없이 설정하기위한 헬퍼
	void SetItemRequest(const EMItemRequestType InRequestType, const int32 InItemRowId, const int32 InItemQuentity, 
						 const FMItemHandle& InItemHandle = {}, UPMInventoryItemInstance* InItemInstance = nullptr)
	{
		RequestType = InRequestType;
		ItemRowId = InItemRowId;
		ItemQuentity = InItemQuentity;
		ItemHandle = InItemHandle;
		ItemInstance = InItemInstance;
	}

	UPROPERTY(BlueprintReadWrite)
	EMItemRequestType RequestType = EMItemRequestType::None;

	UPROPERTY(BlueprintReadWrite)
	int32 ItemRowId = INDEX_NONE;

	UPROPERTY(BlueprintReadWrite)
	int32 ItemQuentity = 0;

	UPROPERTY(BlueprintReadWrite)
	FMItemHandle ItemHandle;

	// RPC에서는 사용 못 함.
	// Replication된 Response에는 설정되어 담겨올 수 있음
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UPMInventoryItemInstance> ItemInstance;
};

UENUM(BlueprintType)
enum class EMItemResponseType : uint8
{
	None,
	TotallyNewItem,
	ChangeItemQuentity,
	Removed,
};

USTRUCT(BlueprintType)
struct FMItemResponse
{
	GENERATED_BODY()

	// 빼먹는 변수 없이 설정하기위한 헬퍼
	void SetItemResponse(const FMItemRequest& InItemRequest, const EMItemResponseType InResponsType, const int32 InItemQuentity, const FMItemHandle& ItemHandle, UPMInventoryItemInstance* ItemInstance)
	{
		ItemRequest = InItemRequest;
		ResponsType = InResponsType;
		ItemQuentity = InItemQuentity;
		ItemRequest.ItemInstance = ItemInstance;
		ItemRequest.ItemHandle = ItemHandle;
	}

	UPROPERTY(BlueprintReadWrite)
	FMItemRequest ItemRequest;
	
	UPROPERTY(BlueprintReadWrite)
	EMItemResponseType ResponsType = EMItemResponseType::None;

	UPROPERTY(BlueprintReadWrite)
	int32 ItemQuentity;
};
