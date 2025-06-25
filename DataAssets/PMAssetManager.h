#pragma once

#include "Engine/AssetManager.h"
#include "PMAssetManager.generated.h"

UCLASS()
class PROJECTM_API UPMAssetManager : public UAssetManager
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UPMAssetManager();

	static UPMAssetManager& Get();

	virtual void StartInitialLoading() final;

	static bool ShouldLogAssetLoads();

	static UObject* SynchronouseLoadAsset(const FSoftObjectPath& AssetPath);

	template<typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	template<typename AssetType>
	static TSubclassOf<AssetType> GetSubclass(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	void AddLoadedAsset(const UObject* Asset);

public:
	UPROPERTY()
	TSet<TObjectPtr<const UObject>> LoadedAssets;

	FCriticalSection SyncObject;
};

template<typename AssetType>
AssetType* UPMAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory /*= true*/)
{
	AssetType* LoadedAsset = nullptr;
	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		LoadedAsset = AssetPointer.Get();
		if (!LoadedAsset)
		{
			LoadedAsset = Cast<AssetType>(SynchronouseLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load asset [%s]"), *AssetPointer.ToString());
		}

		if (LoadedAsset && bKeepInMemory)
		{
			Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
		}
	}

	return LoadedAsset;
}

template<typename AssetType>
TSubclassOf<AssetType> UPMAssetManager::GetSubclass(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory /*= true*/)
{
	TSubclassOf<AssetType> LoadedSubclass;
	const FSoftObjectPath& ASsetPath = AssetPointer.ToSoftObjectPath();

	if (ASsetPath.IsValid())
	{
		LoadedSubclass = AssetPointer.Get();
		if (!LoadedSubclass)
		{
			LoadedSubclass = Cast<UClass>(SynchronouseLoadAsset(ASsetPath));
			ensureAlwaysMsgf(LoadedSubclass, TEXT("Failed to load asset class [%s]"), *AssetPointer.ToString());
		}

		if (LoadedSubclass && bKeepInMemory)
		{
			Get().AddLoadedAsset(Cast<UObject>(LoadedSubclass));
		}
	}

	return LoadedSubclass;
}