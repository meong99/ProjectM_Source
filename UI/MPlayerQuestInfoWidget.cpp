#include "MPlayerQuestInfoWidget.h"
#include "MQuestProgressWidget.h"
#include "MQuestInfoWidget.h"
#include "Components/MPlayerQuestComponent.h"
#include "Blueprint/WidgetTree.h"

UMPlayerQuestInfoWidget::UMPlayerQuestInfoWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMPlayerQuestInfoWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	InitQuest();
}

void UMPlayerQuestInfoWidget::DisplayQuestInfo(const UMQuestDefinition* QuestDefinition, const FMQuestHandle& QuestHandle) const
{
	QuestInfo->DisplayQuestInfo(QuestDefinition, QuestHandle);
}

void UMPlayerQuestInfoWidget::InitQuest()
{
	QuestProgress->InitQuest(QuestInfo);
	QuestInfo->SetVisibility(ESlateVisibility::Collapsed);
	APlayerController* Controller = GetOwningPlayer();
	UMPlayerQuestComponent* QuestComp = Controller ? Controller->FindComponentByClass<UMPlayerQuestComponent>() : nullptr;
	if (QuestComp)
	{
		QuestComp->Delegate_OnChangeQuestState.AddUObject(this, &UMPlayerQuestInfoWidget::UpdateQuest);
	}
}

void UMPlayerQuestInfoWidget::UpdateQuest(const int32 QuestRowId, EMQuestState FromState, EMQuestState ToState)
{
	QuestProgress->UpdateQuest(QuestRowId, FromState, ToState);
}
