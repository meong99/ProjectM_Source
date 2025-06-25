#include "PMGameStateBase.h"
#include "PMExperienceManagerComponent.h"
#include "Player/PMPlayerState.h"
#include "Components/MTradeManager.h"

APMGameStateBase::APMGameStateBase()
{
	ExperienceManagerComponent = CreateDefaultSubobject<UPMExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));
	TradeManager = CreateDefaultSubobject<UMTradeManager>(TEXT("TradeManager"));
}

void APMGameStateBase::BeginPlay()
{
	Super::BeginPlay();
}

void APMGameStateBase::Server_SaveGame_Implementation()
{
	for (APlayerState* PlayerState : PlayerArray)
	{
		APMPlayerState* MyPlayerState = Cast<APMPlayerState>(PlayerState);
		if (MyPlayerState)
		{
			MyPlayerState->Server_SavePlayerData();
		}
	}
}

void APMGameStateBase::Server_LoadGame_Implementation()
{
	for (APlayerState* PlayerState : PlayerArray)
	{
		APMPlayerState* MyPlayerState = Cast<APMPlayerState>(PlayerState);
		if (MyPlayerState)
		{
			MyPlayerState->Server_LoadPlayerData();
		}
	}
}
