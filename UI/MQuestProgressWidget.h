// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MWidgetBase.h"
#include "Types/MQuestTypes.h"
#include "MQuestProgressWidget.generated.h"

class UMQuestSlotWidget;
class UVerticalBox;
class UMQuestDefinition;
class UMPlayerQuestComponent;
class UMQuestInfoWidget;
class UButton;
class UWidgetSwitcher;

UCLASS()
class PROJECTM_API UMQuestProgressWidget : public UMWidgetBase
{
	GENERATED_BODY()

	enum EMButtonState
	{
		Active,
		Deactive,
	};

	/*
	* Overrided Functions
	*/
public:
	UMQuestProgressWidget(const FObjectInitializer& ObjectInitializer);
protected:
	virtual void NativeOnInitialized() override;
	virtual void PreAddToLayer() override;

		/*
	* Member Functions
	*/
public:
	void	InitQuest(UMQuestInfoWidget* InQuestInfo);
	void	UpdateQuest(const int32 QuestRowId, EMQuestState FromState, EMQuestState ToState);
	void	SetInProgressQuests(const TMap<int32, TObjectPtr<UMQuestDefinition>>& QuestDatas, UMQuestInfoWidget* InQuestInfo);
	void	SetStartableQuests(const TMap<int32, TObjectPtr<UMQuestDefinition>>& QuestDatas, UMQuestInfoWidget* InQuestInfo);
	void	SetFinishedQuests(const TMap<int32, TObjectPtr<UMQuestDefinition>>& QuestDatas, UMQuestInfoWidget* InQuestInfo);

protected:
	void	ClearQuestProgress();

	UFUNCTION()
	void	OnClick_InProgress();
	UFUNCTION()
	void	OnClick_Starable();
	UFUNCTION()
	void	OnClick_Finished();

	UMQuestSlotWidget*	GetQuestSlot(const int32 QuestRowId, EMQuestState State, bool bRemove = false) const;
	UVerticalBox*		GetVerticalBox(EMQuestState State) const;

	/*
	* Member Variables
	*/
protected:
	UPROPERTY(BlueprintReadOnly, Category="ProjectM")
	TObjectPtr<UMPlayerQuestComponent> PlayerQuestComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ProjectM")
	TSubclassOf<UMQuestSlotWidget> QuestSlotClass;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UVerticalBox> ProgressVertical;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UVerticalBox> StartableVertical;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UVerticalBox> FinishedVertical;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UButton> InProgressButton_Deactivated;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UButton> StartableButton_Deactivated;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UButton> FinishedButton_Deactivated;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UWidgetSwitcher> InProgressSwitcher;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UWidgetSwitcher> StartableSwitcher;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UWidgetSwitcher> FinishedSwitcher;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UWidgetSwitcher> QuestSlotSwitcher;
};
