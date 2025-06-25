#pragma once

#include "Engine/DataAsset.h"
#include "PMExperienceDefinition.generated.h"

class UGameFeatureAction;
class UPMPawnData;
class UPMExperienceActionSet;

UCLASS()
class PROJECTM_API UPMExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UPMExperienceDefinition();

#if WITH_EDITORONLY_DATA
	// 동적으로 에셋 번들을 Loading할 수 있도록 하는 메소드
	virtual void UpdateAssetBundleData() override;
#endif

/*
* Member Functions
*/
public:
	UPMPawnData* GetDefaultPawnData() const { return DefaultPawnData; }

	const TArray<TObjectPtr<UGameFeatureAction>>& GetActions() const { return Actions; }
	const TArray<TObjectPtr<UPMExperienceActionSet>>& GetActionSets() const { return ActionSets; }

/*
* Member Variables
*/
public:
	UPROPERTY(EditDefaultsOnly, Category = "ProjectM | Experience")
	TArray<FString> GameFeaturesToEnable;

public:
	UPROPERTY(EditDefaultsOnly, Category = "ProjectM | Experience")
	TObjectPtr<UPMPawnData> DefaultPawnData;

	// GameFeatureAction 정의의 집합이며, Gameplay의 컨셉에 맞게 분류할 수 있도록 만들어진 것.
	UPROPERTY(EditDefaultsOnly, Category = "ProjectM | Experience")
	TArray<TObjectPtr<UPMExperienceActionSet>> ActionSets;

	// 분류하지 않고 바로 정의하고싶은 GameFeature의 Action들
	UPROPERTY(EditDefaultsOnly, Category = "ProjectM | Actions")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;
};