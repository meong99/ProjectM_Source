// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MQuestTypes.generated.h"

class UMQuestSlotWidget;

UENUM(BlueprintType)
enum class EMQuestState : uint8
{
	None, 
	Startable,
	InProgress,
	CanFinish,
	Finished,
};

enum EMQuestListButton
{
	InProgress,
	Startable,
	Finished,
};

USTRUCT(BlueprintType)
struct FMQuestItem
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=())
	int32 ItemRowId = INDEX_NONE;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin = 1))
	int32 ItemQuentity = 1;

	UPROPERTY(BlueprintReadOnly)
	int32 TrackedItemNum = 0;
};

USTRUCT(BlueprintType)
struct FMQuestHandle
{
	GENERATED_BODY()

	operator bool() const
	{
		return IsValid();
	}

	bool operator==(const FMQuestHandle& Other) const
	{
		return QuestUid == Other.QuestUid;
	}

	int32 GenerateQuestUid()
	{
		static int32 Uid = 0;

		return Uid++;
	}

	bool IsValid() const
	{
		return QuestUid != INDEX_NONE && QuestState != EMQuestState::None && Slot;
	}

	UPROPERTY(BlueprintReadWrite, Category="ProjectM")
	int32 QuestUid = INDEX_NONE;

	UPROPERTY(BlueprintReadWrite, Category="ProjectM")
	TObjectPtr<UMQuestSlotWidget> Slot;
	
	UPROPERTY(BlueprintReadWrite, Category="ProjectM")
	EMQuestState QuestState = EMQuestState::None;

	UPROPERTY(BlueprintReadWrite, Category="ProjectM")
	TMap<int32, FMQuestItem> TrackedRequiredItem;
};
