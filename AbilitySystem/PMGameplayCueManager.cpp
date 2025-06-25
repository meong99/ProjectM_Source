#include "AbilitySystem/PMGameplayCueManager.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueSet.h"
#include "Engine/AssetManager.h"

UPMGameplayCueManager::UPMGameplayCueManager()
{
}

UPMGameplayCueManager* UPMGameplayCueManager::Get()
{
	return Cast<UPMGameplayCueManager>(UAbilitySystemGlobals::Get().GetGameplayCueManager());
}

const FPrimaryAssetType	UFortAssetManager_GameplayCueRefsType = TEXT("GameplayCueRefs");
const FName				UFortAssetManager_GameplayCueRefsName = TEXT("GameplayCueReferences");
const FName				UFortAssetManager_LoadStateClient = FName(TEXT("Client"));

void UPMGameplayCueManager::RefreshGameplayCuePrimaryAsset()
{
	TArray<FSoftObjectPath> CuePaths;
	UGameplayCueSet* RuntimeGameplayCueSet = GetRuntimeCueSet();
	if (RuntimeGameplayCueSet)
	{
		RuntimeGameplayCueSet->GetSoftObjectPaths(CuePaths);
	}

	// 새로 추가된 에셋의 경로를 CuePaths에 추가
	FAssetBundleData BundleData;
	BundleData.AddBundleAssetsTruncated(UFortAssetManager_LoadStateClient, CuePaths);

	// PrimaryAssetId를 고정된 이름으로 설정하여 에셋 매니저에 추가한다
	FPrimaryAssetId PrimaryAssetId = FPrimaryAssetId(UFortAssetManager_GameplayCueRefsType, UFortAssetManager_GameplayCueRefsName);
	UAssetManager::Get().AddDynamicAsset(PrimaryAssetId, FSoftObjectPath{}, BundleData);
}
