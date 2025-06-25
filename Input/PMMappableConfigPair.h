#pragma once

#include "CoreMinimal.h"
#include "PlayerMappableInputConfig.h"
#include "PMMappableConfigPair.generated.h"

USTRUCT()
struct FPMMappableConfigPair
{
	GENERATED_BODY()

	FPMMappableConfigPair() = default;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UPlayerMappableInputConfig> Config;

	UPROPERTY(EditAnywhere)
	bool bShouldActivateAutomatically = true;
};