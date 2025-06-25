// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/MInventoryTypes.h"
#include "MTradeType.generated.h"

UENUM(BlueprintType)
enum class EMRequestType : uint8
{
	None,
	Trade,
	Give,
	Take,
};

USTRUCT(BlueprintType)
struct FMTradeItemInfo
{
	GENERATED_BODY()

	UPROPERTY()
	int32 ItemRowId;

	UPROPERTY()
	int32 ItemCount;

	UPROPERTY()
	FMItemHandle Handle;
};

USTRUCT(BlueprintType)
struct FMTradeRequest
{
	GENERATED_BODY()

	FMTradeRequest()
	{
		TradeId = GetTradeId();
	}

	int32 GetTradeId() const
	{
		static int32 Id = 0;
		if (TradeId != INDEX_NONE)
		{
			return TradeId;
		}

		return Id++;
	}

	static FMTradeRequest GenerateRequest(EMRequestType InRequestType, const int64 InRequiredGold = 0,
										  const TArray<FMTradeItemInfo>& InRequiredItems = {}, const int64& InGrantGold = 0,
										  const TArray<FMTradeItemInfo>& InGrantItems = {}, const FString& InContextString = "")
	{
		FMTradeRequest Request;

		Request.TradeId = Request.GetTradeId();
		Request.RequestType = InRequestType;
		Request.RequiredGold = InRequiredGold;
		Request.RequiredItems = InRequiredItems;
		Request.GrantGold = InGrantGold;
		Request.GrantItems = InGrantItems;
		Request.ContextString = InContextString;

		return Request;
	}

	UPROPERTY()
	int32 TradeId = INDEX_NONE;

	UPROPERTY()
	EMRequestType RequestType = EMRequestType::None;

	// 상대에게 받을 골드 혹은 교환할 골드
	UPROPERTY()
	int64 RequiredGold = 0;

	// 상대에게 받을 아이템 혹은 교환할 아이템
	UPROPERTY()
	TArray<FMTradeItemInfo> RequiredItems;

	// 상대에게 줄 골드 혹은 교환할 골드
	UPROPERTY()
	int64 GrantGold = 0;

	// 상대에게 줄 아이템 혹은 교환할 아이템
	UPROPERTY()
	TArray<FMTradeItemInfo> GrantItems;

	// 기타 설명용 문자열
	UPROPERTY()
	FString ContextString;
};

UENUM(BlueprintType)
enum class EMResponseType : uint8
{
	None,
	Error,
	Fail,
	Progress,
	Success,
};

USTRUCT(BlueprintType)
struct FMTradeResponse
{
	GENERATED_BODY()

	int32 TradeId = INDEX_NONE;

	EMResponseType ResponseType = EMResponseType::None;

	// 기타 설명용 문자열
	FString ContextString;
};
