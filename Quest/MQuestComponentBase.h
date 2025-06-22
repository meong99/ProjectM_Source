// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MQuestComponentBase.generated.h"

class UMQuestDefinition;

/**
 *
 */
UCLASS(Abstract)
class PROJECTM_API UMQuestComponentBase : public UActorComponent
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UMQuestComponentBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void InitializeComponent() override;

/*
* Member Functions
*/
public:
	const TMap<int32, TObjectPtr<UMQuestDefinition>>& GetQuestDatas() const { return QuestDatas; }

protected:
	void InitQuestData();
	
/*
* Member Variables
*/
protected:
	UPROPERTY(BlueprintReadOnly, Category="ProjectM")
	TMap<int32/*QuestRowId*/, TObjectPtr<UMQuestDefinition>> QuestDatas;
};
