#include "MInteractiveAction_AcceptQuest.h"
#include "System/MDataTableManager.h"
#include "Table/MTable_QuestTable.h"
#include "Engine/Engine.h"
#include "Definitions/MQuestDefinition.h"
#include "Util/MGameplayStatics.h"
#include "UI/MViewportClient.h"
#include "PMGameplayTags.h"
#include "UI/Interaction/MDialogueBoxWidget.h"
#include "MInteractionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/MPlayerQuestComponent.h"
#include "Components/MNpcQuestComponent.h"

UMInteractiveAction_AcceptQuest::UMInteractiveAction_AcceptQuest(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMInteractiveAction_AcceptQuest::InitAction(UMInteractionComponent* InInteractionComponent, AActor* InOwnerActor)
{
	Super::InitAction(InInteractionComponent, InOwnerActor);
}

void UMInteractiveAction_AcceptQuest::ActivateAction()
{
	Super::ActivateAction();

	UMViewportClient* ViewportClient = UMViewportClient::Get(this);
	if (ViewportClient)
	{
		UMDialogueBoxWidget* DialogWidget = Cast<UMDialogueBoxWidget>(ViewportClient->GetWidgetInstance(FPMGameplayTags::Get().UI_Registry_Game_Dialogue));
		UMQuestDefinition* DefCDO = QuestDefinitionClass ? QuestDefinitionClass->GetDefaultObject<UMQuestDefinition>() : nullptr;
		if (DialogWidget && DefCDO)
		{
			FMWidgetInfo WidgetInfo;
			WidgetInfo.WidgetInstigator = this;
			WidgetInfo.WidgetOwnerActor = OwnerActor;
			DialogWidget->SetWidgetInfo(WidgetInfo);

			DialogWidget->InitDialogueWidget(DefCDO->DialogueRowId);
			DialogWidget->SetCallback1([this](){OnClick_Accept();});
			DialogWidget->SetCallback2([this](){OnClick_Refuse();});
			DialogWidget->AddWidgetToLayer(WidgetInfo);
		}
	}
}

void UMInteractiveAction_AcceptQuest::DeactivateAction()
{
	Super::DeactivateAction();
}

void UMInteractiveAction_AcceptQuest::OnClick_Accept()
{
	APlayerController* Controller = UGameplayStatics::GetPlayerController(this, 0);
	UMPlayerQuestComponent* PlayerQuestComponent = Controller ? Controller->FindComponentByClass<UMPlayerQuestComponent>() : nullptr;
	UMNpcQuestComponent* NpcQuestComponent = OwnerActor ? OwnerActor->FindComponentByClass<UMNpcQuestComponent>() : nullptr;
	if (NpcQuestComponent)
	{
		NpcQuestComponent->AcceptQuest(PlayerQuestComponent, QuestRowId);
	}
	if (PlayerQuestComponent)
	{
		PlayerQuestComponent->AcceptQuest(QuestRowId);
	}

	bShouldActivate = false;
}

void UMInteractiveAction_AcceptQuest::OnClick_Refuse()
{
}
