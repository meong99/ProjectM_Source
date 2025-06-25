#include "MNpcTradeComponent.h"
#include "MTradeManager.h"

UMNpcTradeComponent::UMNpcTradeComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMNpcTradeComponent::OnReceivedResponse(const FMTradeRequest& Request, const FMTradeResponse& Response)
{
	// 재구매??
}
