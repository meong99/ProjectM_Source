// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MQuestComponentBase.h"
#include "Types/MQuestTypes.h"
#include "MPlayerQuestComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnChangeQuestState, const int32 QuestRowId, EMQuestState FromState, EMQuestState ToState);
DECLARE_DELEGATE_ThreeParams(FOnChangeQuestStateByQuest, const int32 QuestRowId, EMQuestState FromState, EMQuestState ToState);

UENUM(BlueprintType)
enum class EMQuestResponseType : uint8
{
	None,
	Success,
	NotEnoughItems,
	Error,
};

USTRUCT(BlueprintType)
struct FMQuestResponse 
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ProjectM")
	int32 QuestRowId = INDEX_NONE;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ProjectM")
	EMQuestResponseType ResponseType = EMQuestResponseType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ProjectM")
	EMQuestState FromState;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="ProjectM")
	EMQuestState ToState;
};

/**
 *
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class PROJECTM_API UMPlayerQuestComponent : public UMQuestComponentBase
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UMPlayerQuestComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;

/*
* Member Functions
*/
public:
	void AcceptQuest(const int32 QuestRowId);

	const TSet<int32>& GetInProgressingQuests() const { return InProgressingQuests; }
	const TSet<int32>& GetStartableQuests() const { return StartableQuests; }
	const TSet<int32>& GetFinishedQuests() const { return FinishedQuests; }

	void	AddDelegateOnChangeQuestStateByQuest(const int32 QuestRowId, FOnChangeQuestStateByQuest&& Delegate);
	void	RemoveDelegateOnChangeQuestStateByQuest(const int32 QuestRowId);

	void RequestFinishQuest(const int32 QuestRowId);

protected:
	void UpdateQuestWidget(const int32 QuestRowId, EMQuestState FromState, EMQuestState ToState) const;

	void NotifyOnChangeQuestState(const int32 QuestRowId, EMQuestState FromState, EMQuestState ToState);

	UFUNCTION(Server, Reliable)
	void Server_RequestFinishQuest(const int32 QuestRowId);

	UFUNCTION(Client, Reliable)
	void Client_ResponseFinishQuest(const FMQuestResponse& Response);
/*
* Member Variables
*/

public:
	FOnChangeQuestState Delegate_OnChangeQuestState;

protected:
	TMap<int32, FOnChangeQuestStateByQuest> Delegate_OnChangeQuestStateByQuest;

protected:
	UPROPERTY(BlueprintReadWrite, Category="ProjectM")
	TSet<int32> InProgressingQuests;

	UPROPERTY(BlueprintReadWrite, Category="ProjectM")
	TSet<int32> StartableQuests;

	UPROPERTY(BlueprintReadWrite, Category="ProjectM")
	TSet<int32> FinishedQuests;
};
