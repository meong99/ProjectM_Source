#include "PMExperienceDefinition.h"
#include "GameFeatureAction.h"

UPMExperienceDefinition::UPMExperienceDefinition()
{

}

#if WITH_EDITORONLY_DATA
void UPMExperienceDefinition::UpdateAssetBundleData()
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
