#include "MWalletComponent.h"
#include "Net/UnrealNetwork.h"

UMWalletComponent::UMWalletComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UMWalletComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMWalletComponent, Gold);
}

void UMWalletComponent::SubtractGold(int64 AdjustGold)
{
	if (AdjustGold == 0)
	{
		return;
	}

	if (AdjustGold > 0)
	{
		AdjustGold *= -1;
	}
	int64 Temp = Gold;

	Gold += AdjustGold;
	if (Gold < 0)
	{
		Gold = 0;
	}
	else if (Gold > Temp)
	{
		Gold = Temp;
	}

	OnRep_OnChangeGold(Temp);
}

void UMWalletComponent::AddGold(int64 AdjustGold)
{
	if (AdjustGold == 0)
	{
		return;
	}

	if (AdjustGold < 0)
	{
		AdjustGold *= -1;
	}

	int64 Temp = Gold;

	Gold += AdjustGold;
	if (Gold < 0)
	{
		Gold = 0;
	}
	else if (Gold < Temp)
	{
		Gold = Temp;
	}

	OnRep_OnChangeGold(Temp);
}


void UMWalletComponent::OnRep_OnChangeGold(const int64 OldGold)
{
	Delegate_OnChangeGold.Broadcast(Gold - OldGold, Gold);
}

void UMWalletComponent::ServerDebug_AddGold_Implementation(int64 AdjustGold)
{
	AddGold(AdjustGold);
}

void UMWalletComponent::Debug_AddGold(int64 AdjustGold)
{
	ServerDebug_AddGold(AdjustGold);
}

void UMWalletComponent::ServerDebug_SubtractGold_Implementation(int64 AdjustGold)
{
	SubtractGold(AdjustGold);
}

void UMWalletComponent::Debug_SubtractGold(int64 AdjustGold)
{
	ServerDebug_SubtractGold(AdjustGold);
}
