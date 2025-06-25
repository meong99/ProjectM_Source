#include "MPlayerQuestComponent.h"
#include "Util/MGameplayStatics.h"
#include "UI/MViewportClient.h"
#include "PMGameplayTags.h"
#include "UI/MPlayerQuestInfoWidget.h"
#include "Inventory/PMInventoryManagerComponent.h"
#include "Definitions/MQuestDefinition.h"

UMPlayerQuestComponent::UMPlayerQuestComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMPlayerQuestComponent::InitializeComponent()
{
	Super::InitializeComponent();

#pragma TODO("세이브데이터 추가후 퀘스트 진행상황 로드")
	for (const auto& Iter : QuestDatas)
	{
		UMQuestDefinition* QuestDef = Iter.Value;
		if (QuestDef && QuestDef->bAddToDefault)
		{
			StartableQuests.Add(Iter.Key);
		}
	}
}

void UMPlayerQuestComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() != NM_DedicatedServer)
	{
		UMViewportClient* ViewportClient = UMViewportClient::Get(this);
		if (ViewportClient)
		{
			UMPlayerQuestInfoWidget* QuestInfoWidget = Cast<UMPlayerQuestInfoWidget>(ViewportClient->GetWidgetInstance(FPMGameplayTags::Get().UI_Registry_InputTag_PlayerQuest));
			if (QuestInfoWidget)
			{
				QuestInfoWidget->InitQuest();
			}
		}
	}
}

void UMPlayerQuestComponent::AcceptQuest(const int32 QuestRowId)
{
	int32 RemovedNum = StartableQuests.Remove(QuestRowId);
	if (RemovedNum && InProgressingQuests.Contains(QuestRowId) == false)
	{
		InProgressingQuests.Add(QuestRowId);
		NotifyOnChangeQuestState(QuestRowId, EMQuestState::Startable, EMQuestState::InProgress);
	}
}

void UMPlayerQuestComponent::AddDelegateOnChangeQuestStateByQuest(const int32 QuestRowId, FOnChangeQuestStateByQuest&& Delegate)
{
	Delegate_OnChangeQuestStateByQuest.Add(QuestRowId, MoveTemp(Delegate));
}

void UMPlayerQuestComponent::RemoveDelegateOnChangeQuestStateByQuest(const int32 QuestRowId)
{
	Delegate_OnChangeQuestStateByQuest.Remove(QuestRowId);
}

void UMPlayerQuestComponent::RequestFinishQuest(const int32 QuestRowId)
{
	Server_RequestFinishQuest(QuestRowId);
}

void UMPlayerQuestComponent::NotifyOnChangeQuestState(const int32 QuestRowId, EMQuestState FromState, EMQuestState ToState)
{
	FOnChangeQuestStateByQuest* Delegate = Delegate_OnChangeQuestStateByQuest.Find(QuestRowId);
	if (Delegate)
	{
		Delegate->ExecuteIfBound(QuestRowId, FromState, ToState);
	}
	if (Delegate_OnChangeQuestState.IsBound())
	{
		Delegate_OnChangeQuestState.Broadcast(QuestRowId, FromState, ToState);
	}
}

void UMPlayerQuestComponent::Server_RequestFinishQuest_Implementation(const int32 QuestRowId)
{
	AActor* Owner = GetOwner();
	UMQuestDefinition* QuestDef = QuestDatas.FindRef(QuestRowId);

	FMQuestResponse Response;
	Response.QuestRowId = QuestRowId;
	Response.ResponseType = EMQuestResponseType::Error;

	if (Owner && QuestDef)
	{
		UPMInventoryManagerComponent* InventoryManager = Owner->FindComponentByClass<UPMInventoryManagerComponent>();
		if (InventoryManager)
		{
			bool bIsSuccess = true;

			for (const FMQuestItem& Item : QuestDef->RequiredItems)
			{
				int32 Quentity = InventoryManager->GetItemQuantity(Item.ItemRowId);
				if (Quentity < Item.ItemQuentity)
				{
					bIsSuccess = false;
					break;
				}
			}

			if (bIsSuccess)
			{
				Response.ResponseType = EMQuestResponseType::Success;
				Response.FromState = EMQuestState::CanFinish;
				Response.ToState = EMQuestState::Finished;
			}
			else
			{
				Response.ResponseType = EMQuestResponseType::NotEnoughItems;
			}
		}
	}

	Client_ResponseFinishQuest(Response);
}

void UMPlayerQuestComponent::Client_ResponseFinishQuest_Implementation(const FMQuestResponse& Response)
{
	if (Response.ResponseType == EMQuestResponseType::Success)
	{
		NotifyOnChangeQuestState(Response.QuestRowId, Response.FromState, Response.ToState);
	}
}
