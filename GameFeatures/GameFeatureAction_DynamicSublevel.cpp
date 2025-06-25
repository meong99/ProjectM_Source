#include "GameFeatureAction_DynamicSublevel.h"
#include "GameFeaturesSubsystem.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Kismet/GameplayStatics.h"

UGameFeatureAction_DynamicSublevel::UGameFeatureAction_DynamicSublevel()
{
}

void UGameFeatureAction_DynamicSublevel::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
{
	UWorld* World = WorldContext.World();
	UGameInstance* GameInstance = WorldContext.OwningGameInstance;
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	if ((GameInstance != nullptr) && (World != nullptr) && World->IsGameWorld())
	{
		if (!LoadedLevel && !LevelData.Sublevel.IsValid())
		{
			LevelData.Sublevel.LoadSynchronous();
			LoadedLevel = LevelData.Sublevel.Get();
		}
		if (LoadedLevel)
		{
			bool bSuccessed = false;
			Streaming = ULevelStreamingDynamic::LoadLevelInstance(World, LoadedLevel->GetMapName(), FVector{}, FRotator{}, bSuccessed);
		}
	}
}

void UGameFeatureAction_DynamicSublevel::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	if (Streaming)
	{
		Streaming->ClearLoadedLevel();
	}
}
