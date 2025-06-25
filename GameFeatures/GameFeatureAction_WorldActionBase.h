#pragma once

#include "GameFeatureAction.h"
#include "GameFeaturesSubsystem.h"
#include "GameFeatureAction_WorldActionBase.generated.h"

/*
* GameFeature같은 경우에는 EngineSubsystem에 종속적이기에 어떤 World에 적용되어야 하는지 알아야한다.
* 그래서 WorldActionBase를 상속받아서 사용하도록 하고, WorldActionBase는 WorldContext를 기반으로 Action을 적용한다.
*/
UCLASS()
class PROJECTM_API UGameFeatureAction_WorldActionBase : public UGameFeatureAction
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UGameFeatureAction_WorldActionBase();

	// GameFeature가 모두 활성화되고나서 WorldContext를 넘겨줬었는데 그 Context를 기반으로 적용할 월드를 찾는다.
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;

/*
* Member Functions
*/
public:
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) PURE_VIRTUAL(UGameFeatureAction_WorldActionBase::AddToWorld, );

/*
* Member Variables
*/
public:
};