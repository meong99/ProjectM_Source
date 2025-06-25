#include "PMGameFeaturePolicy.h"
#include "GameFeaturesSubsystem.h"
#include "GameFeatureAction_AddGameplayCuePath.h"
#include "GameFeatureData.h"
#include "AbilitySystem/PMGameplayCueManager.h"
#include "GameplayCueSet.h"

UPMGameFeaturePolicy::UPMGameFeaturePolicy()
{

}

void UPMGameFeaturePolicy::InitGameFeatureManager()
{
	// Observer에 CuePaths등록
	Observers.Add(NewObject<UPMGameFeature_AddGameplayCuePaths>());

	// 등록된 Observers를 모두 Subsystem에 등록
	UGameFeaturesSubsystem& Subsystem = UGameFeaturesSubsystem::Get();
	for (UObject* Observer : Observers)
	{
		Subsystem.AddObserver(Observer);
	}

	// 등록된 Observer들이 Gamefeature가 초기화 되는 과정에서 반영될 것
	Super::InitGameFeatureManager();
}

void UPMGameFeaturePolicy::ShutdownGameFeatureManager()
{
	Super::ShutdownGameFeatureManager();

	UGameFeaturesSubsystem& Subsystem = UGameFeaturesSubsystem::Get();
	for (UObject* Observer : Observers)
	{
		Subsystem.RemoveObserver(Observer);
	}

	Observers.Empty();
}

void UPMGameFeature_AddGameplayCuePaths::OnGameFeatureRegistering(const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL)
{
	// PluginName을 활용하여, PluginRootPath를 계산
	const FString PluginRootPath = TEXT("/") + PluginName;

	// GameFeatureData의 Action을 순회
	for (const UGameFeatureAction* Action : GameFeatureData->GetActions())
	{
		// 그 중 _AddGameplayCuepath를 찾아서 처리
		if (const UGameFeatureAction_AddGameplayCuePath* AddGameplayCueGFA = Cast<UGameFeatureAction_AddGameplayCuePath>(Action))
		{
			const TArray<FDirectoryPath>& DirsToAdd = AddGameplayCueGFA->DirectoryPathsToAdd;

			// GameplayCueManager를 가져와서 GFA에 등록된 DirsToAdd를 추가하면서 GCM의 데이터가 업데이트 되도록 진행
			if (UPMGameplayCueManager* GCM = UPMGameplayCueManager::Get())
			{
				UGameplayCueSet* RuntimeGameplayCueSet = GCM->GetRuntimeCueSet();
				const int32 PreInitializeNumCues = RuntimeGameplayCueSet ? RuntimeGameplayCueSet->GameplayCueData.Num() : 0;

				for (const FDirectoryPath& Directory : DirsToAdd)
				{
					FString MutablePath = Directory.Path;

					// PluginPackagePath를 한 번 보정해줌 -> 보정된 결과는 MutablePath로 들어옴
					UGameFeaturesSubsystem::FixPluginPackagePath(MutablePath, PluginRootPath, false);

					// GCM의 RuntimeGameplayCueObjectLibrary의 Path추가
					GCM->AddGameplayCueNotifyPath(MutablePath, /*bShouldRescanCueAssets=*/false);
				}

				// 초기화 시켜주고 (새로 경로가 추가되었으니 호출해줘야함
				if (!DirsToAdd.IsEmpty())
				{
					GCM->InitializeRuntimeObjectLibrary();
				}

				// Cue의 에셋이 Pre와 Post랑 비교해서 갯수가 달라져 있다면, 명시적으로 RefreshGameplayCuePrimaryAsset 호출
				const int32 PostInitializeNumCues = RuntimeGameplayCueSet ? RuntimeGameplayCueSet->GameplayCueData.Num() : 0;
				if (PreInitializeNumCues != PostInitializeNumCues)
				{
					// RefreshGameplayCuePrimaryAsset함수는 추가된 GCN을 AssetManager에 등록해주는 역할을 한다
					GCM->RefreshGameplayCuePrimaryAsset();
				}
			}
		}
	}
}

void UPMGameFeature_AddGameplayCuePaths::OnGameFeatureUnregistering(const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL)
{
	const FString PluginRootPath = TEXT("/") + PluginName;
	for (const UGameFeatureAction* Action : GameFeatureData->GetActions())
	{
		if (const UGameFeatureAction_AddGameplayCuePath* AddGameplayCueGFA = Cast<UGameFeatureAction_AddGameplayCuePath>(GameFeatureData))
		{
			const TArray<FDirectoryPath>& DirsToAdd = AddGameplayCueGFA->DirectoryPathsToAdd;
			if (UPMGameplayCueManager* GCM = UPMGameplayCueManager::Get())
			{
				int32 NumRemoved = 0;
				for (const FDirectoryPath& Directory : DirsToAdd)
				{
					FString MutablePath = Directory.Path;
					UGameFeaturesSubsystem::FixPluginPackagePath(MutablePath, PluginRootPath, false);
					NumRemoved += GCM->RemoveGameplayCueNotifyPath(MutablePath, /** bShouldRescanCueAssets=*/ false);
				}

				ensure(NumRemoved == DirsToAdd.Num());

				if (NumRemoved > 0)
				{
					GCM->InitializeRuntimeObjectLibrary();
				}
			}
		}
	}
}
