#include "PMGameplayAbilityTargetData_SingleTarget.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(PMGameplayAbilityTargetData_SingleTarget)

bool FPMGameplayAbilityTargetData_SingleTargetHit::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayAbilityTargetData_SingleTargetHit::NetSerialize(Ar, Map, bOutSuccess);

	Ar << CartridgeID;

	return true;
}
