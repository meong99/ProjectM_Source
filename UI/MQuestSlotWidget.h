// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MWidgetBase.h"
#include "Types/MQuestTypes.h"
#include "MQuestSlotWidget.generated.h"

class UTextBlock;
class UMQuestDefinition;
class UMQuestInfoWidget;

UCLASS()
class PROJECTM_API UMQuestSlotWidget : public UMWidgetBase
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UMQuestSlotWidget(const FObjectInitializer& ObjectInitializer);
protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

/*
* Member Functions
*/
public:
	void	InitQuestSlot(UMQuestInfoWidget* InQuestInfo, const UMQuestDefinition* InQuestDefinition, EMQuestState InQuestState);
	void	UpdateSlot(EMQuestState InQuestState);
	void	OnClick_FinishButton();

	const FMQuestHandle&		GetHandle() const { return QuestHandle; }
	const UMQuestDefinition*	GetQuestDefinition() const { return QuestDefinition; }
protected:
	void	SetQuestName(const FText& InQuestName);
	void	StartQuestTracking();
	void	FinishQuestTracking();
	void	UpdateQuestInfo();
/*
* Member Variables
*/
protected:
	UPROPERTY(BlueprintReadOnly, Category="ProjectM")
	FMQuestHandle QuestHandle;

	UPROPERTY(BlueprintReadOnly, Category="ProjectM")
	TObjectPtr<UMQuestInfoWidget> QuestInfo;
	
	UPROPERTY(BlueprintReadOnly, Category = "ProjectM")
	const UMQuestDefinition* QuestDefinition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UTextBlock> QuestName;

	FDelegateHandle Handle_AddItem;
	FDelegateHandle Handle_RemoveItem;
};
