#include "GameFeatureAction_AddGameplayCuePath.h"

UGameFeatureAction_AddGameplayCuePath::UGameFeatureAction_AddGameplayCuePath()
{
	DirectoryPathsToAdd.Add(FDirectoryPath{ TEXT("/GameplayCues") });
}