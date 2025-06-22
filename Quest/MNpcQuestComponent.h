// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MQuestComponentBase.h"
#include "MPlayerQuestComponent.h"
#include "Types/MQuestTypes.h"
#include "MNpcQuestComponent.generated.h"

class UMPlayerQuestComponent;

/**
 *
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class PROJECTM_API UMNpcQuestComponent : public UMQuestComponentBase
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UMNpcQuestComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void InitializeComponent() override;

/*
* Member Functions
*/
public:
	void AcceptQuest(UMPlayerQuestComponent* InPlayerQuestComponent, const int32 QuestRowId);
	void OnChangeQuestState(const int32 QuestRowId, EMQuestState FromState, EMQuestState ToState);
	
/*
* Member Variables
*/
protected:
	UPROPERTY(BlueprintReadOnly, Category="ProjectM")
	TSet<int32> InProgressQuest;

	TWeakObjectPtr<UMPlayerQuestComponent> PlayerQuestComponent;
};
