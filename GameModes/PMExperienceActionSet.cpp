#include "PMExperienceActionSet.h"
#include "GameFeatureAction.h"

UPMExperienceActionSet::UPMExperienceActionSet()
{
}

#if WITH_EDITORONLY_DATA
void UPMExperienceActionSet::UpdateAssetBundleData()
{
	Super::UpdateAssetBundleData();

	for (UGameFeatureAction* Action : Actions)
	{
		if (Action)
		{
			Action->AddAdditionalAssetBundleData(AssetBundleData);
		}
	}
}
#endif
