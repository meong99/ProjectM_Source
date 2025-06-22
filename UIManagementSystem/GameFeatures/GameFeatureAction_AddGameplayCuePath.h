#pragma once

#include "GameFeatureAction.h"
#include "UObject/NoExportTypes.h"
#include "GameFeatureAction_AddGameplayCuePath.generated.h"

/*
* GameplayCue같은 경우에는 다른 Action과는 다르게 World를 구분지을 필요가 없기 때문에 WorldActionBase를 상속받지 않는다.
*/
UCLASS()
class PROJECTM_API UGameFeatureAction_AddGameplayCuePath : public UGameFeatureAction
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UGameFeatureAction_AddGameplayCuePath();

/*
* Member Functions
*/
public:

/*
* Member Variables
*/
public:
	// 스캔을 위해 GameplayCuewNotify의 에셋 경로를 추가하는 곳이다.
	UPROPERTY(EditAnyWhere, Category = "GameFeature | GameplayCue")
	TArray<FDirectoryPath> DirectoryPathsToAdd;
};