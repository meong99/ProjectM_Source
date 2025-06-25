#include "MNpcQuestComponent.h"
#include "MPlayerTradeComponent.h"
#include "Definitions/MQuestDefinition.h"
#include "GameFramework/Actor.h"

UMNpcQuestComponent::UMNpcQuestComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMNpcQuestComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UMNpcQuestComponent::AcceptQuest(UMPlayerQuestComponent* InPlayerQuestComponent, const int32 QuestRowId)
{
	PlayerQuestComponent = InPlayerQuestComponent;
	if (PlayerQuestComponent.IsValid())
	{
		PlayerQuestComponent->AddDelegateOnChangeQuestStateByQuest(QuestRowId, FOnChangeQuestStateByQuest::CreateUObject(this, &UMNpcQuestComponent::OnChangeQuestState));
	}
}

void UMNpcQuestComponent::OnChangeQuestState(const int32 QuestRowId, EMQuestState FromState, EMQuestState ToState)
{
	if (FromState == EMQuestState::Startable && ToState == EMQuestState::InProgress)
	{
		InProgressQuest.Add(QuestRowId);
	}
	else if (ToState == EMQuestState::Finished && PlayerQuestComponent.IsValid())
	{
		PlayerQuestComponent->RemoveDelegateOnChangeQuestStateByQuest(QuestRowId);
		InProgressQuest.Remove(QuestRowId);

		UMQuestDefinition* QuestDef = QuestDatas.FindRef(QuestRowId);
		if (QuestDef)
		{
			UMPlayerTradeComponent* PlayerTradeComponent = PlayerQuestComponent->GetOwner()->FindComponentByClass<UMPlayerTradeComponent>();
			if (PlayerTradeComponent)
			{
				FMTradeRequest Request = QuestDef->CreateTradeRequest();

				PlayerTradeComponent->Server_OnRequestSimpleTrading(GetOwner(), Request);
			}
		}

		if (InProgressQuest.IsEmpty())
		{
			PlayerQuestComponent = nullptr;
		}
	}
}
