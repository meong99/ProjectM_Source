#pragma once

#include "Engine/DataAsset.h"
#include "PMExperienceActionSet.generated.h"

class UGameFeatureAction;

UCLASS()
class PROJECTM_API UPMExperienceActionSet : public UPrimaryDataAsset
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UPMExperienceActionSet();

#if WITH_EDITORONLY_DATA
// 동적으로 에셋 번들을 Loading할 수 있도록 하는 메소드
	virtual void UpdateAssetBundleData() override;
#endif

/*
* Member Functions
*/
public:

/*
* Member Variables
*/
public:

	UPROPERTY(EditAnywhere, Category = "ProjectM | ActionsToPerform")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;
};