#include "MTradeComponentBase.h"

UMTradeComponentBase::UMTradeComponentBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UMTradeComponentBase::Server_OnRequestSimpleTrading_Implementation(AActor* Requestor, const FMTradeRequest& Request)
{
	ensure(false);
	MCHAE_ERROR("Server_OnRequestSimpleTrading function in UMTradeComponentBase does nothing by default! if you want to use it, have to override it");
}

void UMTradeComponentBase::Server_OnRequestSimpleDataGrant_Implementation(AActor* Requestor, const FMTradeRequest& Request)
{
	ensure(false);
	MCHAE_ERROR("OnRequestSimpleDataGrant function in UMTradeComponentBase does nothing by default! if you want to use it, have to override it");
}

const FMTradeResponse UMTradeComponentBase::MakeErrorResponse(const FMTradeRequest& Request, const FString& ContextString)
{
	FMTradeResponse Response;

	Response.TradeId = Request.TradeId;
	Response.ResponseType = EMResponseType::Error;
	Response.ContextString = ContextString;

	return Response;
}

const FMTradeResponse UMTradeComponentBase::MakeFailResponse(const FMTradeRequest& Request, const FString& ContextString)
{
	FMTradeResponse Response;

	Response.TradeId = Request.TradeId;
	Response.ResponseType = EMResponseType::Fail;
	Response.ContextString = ContextString;

	return Response;
}

const FMTradeResponse UMTradeComponentBase::MakeSuccessResponse(const FMTradeRequest& Request, const FString& ContextString)
{
	FMTradeResponse Response;

	Response.TradeId = Request.TradeId;
	Response.ResponseType = EMResponseType::Success;
	Response.ContextString = ContextString;

	return Response;
}
