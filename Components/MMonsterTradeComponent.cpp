#include "MMonsterTradeComponent.h"

UMMonsterTradeComponent::UMMonsterTradeComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMMonsterTradeComponent::OnReceivedResponse(const FMTradeRequest& Request, const FMTradeResponse& Response)
{
	// 실패라면 그냥 아이템 떨구기?
}
