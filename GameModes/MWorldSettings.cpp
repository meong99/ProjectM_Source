#include "MWorldSettings.h"
#include "Engine/AssetManager.h"

AMWorldSettings::AMWorldSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

FPrimaryAssetId AMWorldSettings::GetDefaultGameplayExperience() const
{
	FPrimaryAssetId Result;
	if (!DefaultGameplayExperience.IsNull())
	{
		Result = UAssetManager::Get().GetPrimaryAssetIdForPath(DefaultGameplayExperience.ToSoftObjectPath());

		if (!Result.IsValid())
		{
			MCHAE_ERROR("%s.DefaultGameplayExperience is %s but that failed to resolve into an asset ID (you might need to add a path to the Asset Rules in your game feature plugin or project settings",
				*GetPathNameSafe(this), *DefaultGameplayExperience.ToString());
		}
	}
	return Result;
}
