#include "MMonsterDefinition.h"
#include "Engine/Engine.h"

UMMonsterDefinition::UMMonsterDefinition()
{
}

FMTradeRequest UMMonsterDefinition::CreateTradeRequest() const
{
	FMTradeRequest Request = FMTradeRequest::GenerateRequest(EMRequestType::Trade);
#pragma TODO("확률적용해야함")
	for (const FMDropInfo& DropInfo : MonsterInfo.ItemDropTable)
	{
		Request.GrantItems.Add({ DropInfo.ItemId, DropInfo.ItemCount });
	}

	Request.GrantGold = MonsterInfo.MonsterReward;

	return Request;
}
