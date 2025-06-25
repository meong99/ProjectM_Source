#include "PMAssetManager.h"
#include "Engine/Engine.h"
#include "Misc/CommandLine.h"
#include "Stats/StatsMisc.h"
#include "PMGameplayTags.h"

UPMAssetManager::UPMAssetManager()
{
}

UPMAssetManager& UPMAssetManager::Get()
{
	check(GEngine);

	// 우리는 AssetManager를 UEngine의 GEngine의 AssetManager의 Class를 오버라이드 했기 때문에, GEngine에 Asset Manager가 있음
	if (UPMAssetManager* Singleton = Cast<UPMAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	// 에셋 매니저 UPMAssetManager로 추가해야함!
	MCHAE_FETAL("invalid AssetManagerClassname in DefaultEngine.ini(project settings); it must be PMAssetManager");

	// 위의 UE_LOG의 Fatal로 인해, Crash 나기 때문에 도달하지 않지만 컴파일을 위해 더미로 리턴
	return *NewObject<UPMAssetManager>();
}

void UPMAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FPMGameplayTags::InitializeNativeTags();
}

bool UPMAssetManager::ShouldLogAssetLoads()
{
	const TCHAR* CommandLineContent = FCommandLine::Get();
	static bool bLogAssetLoads = FParse::Param(CommandLineContent, TEXT("LogAssetLoads"));

	return bLogAssetLoads;
}

UObject* UPMAssetManager::SynchronouseLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		TUniquePtr<FScopeLogTime> LogTimePtr;
		if (ShouldLogAssetLoads())
		{
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("synchronous loaded assets [%s"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}

		if (UAssetManager::IsInitialized())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath);
		}

		return AssetPath.TryLoad();
	}

	return nullptr;
}

void UPMAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock Lock(&SyncObject);
		LoadedAssets.Add(Asset);
	}
}

