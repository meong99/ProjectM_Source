#include "MPawnComponentBase.h"
#include "Engine/NetConnection.h"
#include "GameFramework/PlayerController.h"
#include "Engine/NetDriver.h"
#include "Engine/PackageMapClient.h"
#include "Util/MGameplayStatics.h"

UMPawnComponentBase::UMPawnComponentBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

bool UMPawnComponentBase::IsNetConnectionReady() const
{
	return UMGameplayStatics::CheckNetGuid(this, this);
}
