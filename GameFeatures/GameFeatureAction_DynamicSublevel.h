#pragma once

#include "GameFeatureAction.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "GameFeatureAction_DynamicSublevel.generated.h"

struct FComponentRequestHandle;
class UWorld;
class ULevelStreamingDynamic;

USTRUCT(BlueprintType)
struct FMDynamicLevelData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta=(AllowedTypes = "Map"))
	TSoftObjectPtr<UWorld> Sublevel;
};

/*
*/
UCLASS()
class PROJECTM_API UGameFeatureAction_DynamicSublevel : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

	/*
	* Overrided Function
	*/
public:
	UGameFeatureAction_DynamicSublevel();

	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	/*
	* Member Functions
	*/
public:

/*
* Member Variables
*/
public:
	struct FPerContextData
	{
		TArray<TSharedPtr<FComponentRequestHandle>> ComponentRequests;
	};

	UPROPERTY(EditDefaultsOnly)
	FMDynamicLevelData LevelData;

	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;

	UPROPERTY()
	TObjectPtr<UWorld> LoadedLevel;

	UPROPERTY()
	ULevelStreamingDynamic* Streaming;
};