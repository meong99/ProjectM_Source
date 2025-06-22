#include "MQuestDefinition.h"
#include "System/MDataTableManager.h"
#include "Character/NPC/MNpcDefinition.h"
#include "Character/NPC/MNpcBase.h"
#include "Interaction/MInteractionComponent.h"
#include "Interaction/MInteractiveAction_AcceptQuest.h"

UMQuestDefinition::UMQuestDefinition(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/) : Super(ObjectInitializer)
{

}

#if WITH_EDITOR
void UMQuestDefinition::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UMNpcDefinition* NpcDef = UMDataTableManager::GetDefinitionDefaultObject<UMNpcDefinition>(this, OwnerNpcRowId);
	if (NpcDef)
	{
		NpcDef->Modify();
		if (NpcDef->NpcClass)
		{
			AMNpcBase* NpcCDO = NpcDef->NpcClass->GetDefaultObject<AMNpcBase>();
			if (NpcCDO)
			{
				NpcCDO->SetNpcRowId(OwnerNpcRowId);
			}
		}

		auto IsDuplicate = [&, this]()->bool
		{
			for (const UMInteractiveAction_OnInteractionBase* Action : NpcDef->Action_OnInteract)
			{
				const UMInteractiveAction_AcceptQuest* QuestAction = Cast<UMInteractiveAction_AcceptQuest>(Action);
				if (QuestAction && QuestAction->QuestRowId == RowId)
				{
					return true;
				}
			}

			return false;
		};

		UMInteractiveAction_AcceptQuest* QuestAction = NewObject<UMInteractiveAction_AcceptQuest>(NpcDef, UMInteractiveAction_AcceptQuest::StaticClass(), NAME_None, RF_Transactional);
		if (QuestAction && !IsDuplicate())
		{
			NpcDef->Action_OnInteract.Empty();
			QuestAction->ActionName = QuestName;
			QuestAction->QuestRowId = RowId;
			QuestAction->QuestDefinitionClass = GetClass();
			NpcDef->Action_OnInteract.Add(QuestAction);
		}
	}
}

#endif

FMTradeRequest UMQuestDefinition::CreateTradeRequest() const
{
	FMTradeRequest Request = FMTradeRequest::GenerateRequest(EMRequestType::Trade);
	for (const FMQuestItem& RequiredItem : RequiredItems)
	{
		Request.RequiredItems.Add({ RequiredItem.ItemRowId, RequiredItem.ItemQuentity });
	}
	for (const FMQuestItem& RewardItem : RewardItems)
	{
		Request.GrantItems.Add({ RewardItem.ItemRowId, RewardItem.ItemQuentity });
	}
	Request.GrantGold = RewardGold;

	return Request;
}
