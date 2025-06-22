#include "MQuestProgressWidget.h"
#include "Definitions/MQuestDefinition.h"
#include "MQuestSlotWidget.h"
#include "Components/VerticalBox.h"
#include "Components/MPlayerQuestComponent.h"
#include "GameFramework/PlayerController.h"
#include "MQuestInfoWidget.h"
#include "Components/Widget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"

UMQuestProgressWidget::UMQuestProgressWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMQuestProgressWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UMQuestProgressWidget::PreAddToLayer()
{
	InProgressButton_Deactivated->OnClicked.AddDynamic(this, &UMQuestProgressWidget::OnClick_InProgress);
	StartableButton_Deactivated->OnClicked.AddDynamic(this, &UMQuestProgressWidget::OnClick_Starable);
	FinishedButton_Deactivated->OnClicked.AddDynamic(this, &UMQuestProgressWidget::OnClick_Finished);
}

void UMQuestProgressWidget::InitQuest(UMQuestInfoWidget* InQuestInfo)
{
	if (!InQuestInfo)
	{
		ensure(false);
		MCHAE_ERROR("PlayerQuestInfo is not valid");
		return;
	}

	APlayerController* Controller = GetOwningPlayer<APlayerController>();
	if (Controller)
	{
		PlayerQuestComponent = Controller->FindComponentByClass<UMPlayerQuestComponent>();
		if (!PlayerQuestComponent)
		{
			MCHAE_ERROR("QuestComponent is not valid");
			ensure(false);
			return;
		}
	}

	ClearQuestProgress();

	const TMap<int32, TObjectPtr<UMQuestDefinition>>& QuestDatas = PlayerQuestComponent->GetQuestDatas();
	SetInProgressQuests(QuestDatas, InQuestInfo);
	SetStartableQuests(QuestDatas, InQuestInfo);
	SetFinishedQuests(QuestDatas, InQuestInfo);
}

void UMQuestProgressWidget::UpdateQuest(const int32 QuestRowId, EMQuestState FromState, EMQuestState ToState)
{
	UMQuestSlotWidget* QuestSlot = GetQuestSlot(QuestRowId, FromState, true);
	if (QuestSlot)
	{
		UVerticalBox* VerticalBox = GetVerticalBox(ToState);
		if (VerticalBox)
		{
			VerticalBox->AddChild(QuestSlot);
			QuestSlot->UpdateSlot(ToState);
		}
	}
}

void UMQuestProgressWidget::SetInProgressQuests(const TMap<int32, TObjectPtr<UMQuestDefinition>>& QuestDatas, UMQuestInfoWidget* InQuestInfo)
{
	const TSet<int32>& InProgressingQuests = PlayerQuestComponent->GetInProgressingQuests();

	for (const int32 QuestRowId : InProgressingQuests)
	{
		UMQuestDefinition* QuestDef = QuestDatas.FindRef(QuestRowId);
		if (QuestDef)
		{
			UMQuestSlotWidget* QuestSlot = CreateWidget<UMQuestSlotWidget>(GetOwningPlayer(), QuestSlotClass);
			if (QuestSlot)
			{
				QuestSlot->InitQuestSlot(InQuestInfo, QuestDef, EMQuestState::InProgress);
				ProgressVertical->AddChildToVerticalBox(QuestSlot);
			}
		}
	}
}

void UMQuestProgressWidget::SetStartableQuests(const TMap<int32, TObjectPtr<UMQuestDefinition>>& QuestDatas, UMQuestInfoWidget* InQuestInfo)
{
	const TSet<int32>& StartableQuests = PlayerQuestComponent->GetStartableQuests();

	for (const int32 QuestRowId : StartableQuests)
	{
		UMQuestDefinition* QuestDef = QuestDatas.FindRef(QuestRowId);
		if (QuestDef)
		{
			UMQuestSlotWidget* QuestSlot = CreateWidget<UMQuestSlotWidget>(GetOwningPlayer(), QuestSlotClass);
			if (QuestSlot)
			{
				QuestSlot->InitQuestSlot(InQuestInfo, QuestDef, EMQuestState::Startable);
				StartableVertical->AddChildToVerticalBox(QuestSlot);
			}
		}
	}
}

void UMQuestProgressWidget::SetFinishedQuests(const TMap<int32, TObjectPtr<UMQuestDefinition>>& QuestDatas, UMQuestInfoWidget* InQuestInfo)
{
	const TSet<int32>& FinishedQuests = PlayerQuestComponent->GetFinishedQuests();

	for (const int32 QuestRowId : FinishedQuests)
	{
		UMQuestDefinition* QuestDef = QuestDatas.FindRef(QuestRowId);
		if (QuestDef)
		{
			UMQuestSlotWidget* QuestSlot = CreateWidget<UMQuestSlotWidget>(GetOwningPlayer(), QuestSlotClass);
			if (QuestSlot)
			{
				QuestSlot->InitQuestSlot(InQuestInfo, QuestDef, EMQuestState::Finished);
				FinishedVertical->AddChildToVerticalBox(QuestSlot);
			}
		}
	}
}

void UMQuestProgressWidget::ClearQuestProgress()
{
	ProgressVertical->ClearChildren();
	StartableVertical->ClearChildren();
}

void UMQuestProgressWidget::OnClick_InProgress()
{
	InProgressSwitcher->SetActiveWidgetIndex((int32)EMButtonState::Active);
	StartableSwitcher->SetActiveWidgetIndex((int32)EMButtonState::Deactive);
	FinishedSwitcher->SetActiveWidgetIndex((int32)EMButtonState::Deactive);
	QuestSlotSwitcher->SetActiveWidgetIndex((int32)EMQuestListButton::InProgress);
}

void UMQuestProgressWidget::OnClick_Starable()
{
	InProgressSwitcher->SetActiveWidgetIndex((int32)EMButtonState::Deactive);
	StartableSwitcher->SetActiveWidgetIndex((int32)EMButtonState::Active);
	FinishedSwitcher->SetActiveWidgetIndex((int32)EMButtonState::Deactive);
	QuestSlotSwitcher->SetActiveWidgetIndex((int32)EMQuestListButton::Startable);
}

void UMQuestProgressWidget::OnClick_Finished()
{
	InProgressSwitcher->SetActiveWidgetIndex((int32)EMButtonState::Deactive);
	StartableSwitcher->SetActiveWidgetIndex((int32)EMButtonState::Deactive);
	FinishedSwitcher->SetActiveWidgetIndex((int32)EMButtonState::Active);
	QuestSlotSwitcher->SetActiveWidgetIndex((int32)EMQuestListButton::Finished);
}

UMQuestSlotWidget* UMQuestProgressWidget::GetQuestSlot(const int32 QuestRowId, EMQuestState State, bool bRemove) const
{
	UVerticalBox* Vertical = GetVerticalBox(State);
	if (!Vertical)
	{
		ensure(false);
		return nullptr;
	}

	TArray<UWidget*> Children = Vertical->GetAllChildren();

	for (int32 i = 0; i < Children.Num(); i++)
	{
		UMQuestSlotWidget* QuestSlot = Cast<UMQuestSlotWidget>(Children[i]);
		if (QuestSlot)
		{
			const UMQuestDefinition* Def = QuestSlot->GetQuestDefinition();
			if (Def && Def->RowId == QuestRowId)
			{
				if (bRemove)
				{
					Vertical->RemoveChildAt(i);
				}
				return QuestSlot;
			}
		}
	}

	return nullptr;
}

UVerticalBox* UMQuestProgressWidget::GetVerticalBox(EMQuestState State) const
{
	switch (State)
	{
	case EMQuestState::None:
		return nullptr;
		break;
	case EMQuestState::Startable:
		return StartableVertical;
		break;
	case EMQuestState::InProgress:
		return ProgressVertical;
		break;
	case EMQuestState::CanFinish:
		return ProgressVertical;
		break;
	case EMQuestState::Finished:
		return FinishedVertical;
		break;
	default:
		return nullptr;
		break;
	}
}
