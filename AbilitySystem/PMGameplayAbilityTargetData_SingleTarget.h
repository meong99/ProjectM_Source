#pragma once

#include "Abilities/GameplayAbilityTargetTypes.h"
#include "PMGameplayAbilityTargetData_SingleTarget.generated.h"

USTRUCT()
struct FPMGameplayAbilityTargetData_SingleTargetHit : public FGameplayAbilityTargetData_SingleTargetHit
{
	GENERATED_BODY()
public:
	FPMGameplayAbilityTargetData_SingleTargetHit()
		: CartridgeID(-1)
	{}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FPMGameplayAbilityTargetData_SingleTargetHit::StaticStruct();
	}

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	// 탄약 ID (카트리지)
	UPROPERTY()
	int32 CartridgeID;
};

template<>
struct TStructOpsTypeTraits<FPMGameplayAbilityTargetData_SingleTargetHit> : public TStructOpsTypeTraitsBase2<FPMGameplayAbilityTargetData_SingleTargetHit>
{
	enum
	{
		WithNetSerializer = true	// For now this is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};
