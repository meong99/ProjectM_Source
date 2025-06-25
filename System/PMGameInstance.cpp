#include "PMGameInstance.h"
#include "Components/GameFrameworkComponentManager.h"
#include "PMGameplayTags.h"
#include "GameFeaturesSubsystem.h"
#include "GameFeatureData.h"

UPMGameInstance::UPMGameInstance()
{
}

void UPMGameInstance::Init()
{
	Super::Init();

	// 컴포넌트를 붙일 때 특정 컴포넌트는 다른 컴포넌트에 종속성이 있을 수 있는데 GameFeature를 통해서 컴포넌트를 붙일 때 순서가 보장되지 않는다.
	// 하지만 여기에서 Tag를 등록하면 현재 상태에 따른 이벤트 발생을 통해서 컴포넌트의 초기화 순서를 보장할 수 있다.
	// 아래와 같이 이전 태그가 적용되어 있어야 다음 태그로 넘어가는 형태.
	UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>(this);
	if (ensure(ComponentManager))
	{
		const FPMGameplayTags& GameplayTags = FPMGameplayTags::Get();

		ComponentManager->RegisterInitState(GameplayTags.InitState_Spawned, false, FGameplayTag{});
		ComponentManager->RegisterInitState(GameplayTags.InitState_DataAvailable, false, GameplayTags.InitState_Spawned);
		ComponentManager->RegisterInitState(GameplayTags.InitState_DataInitialized, false, GameplayTags.InitState_DataAvailable);
		ComponentManager->RegisterInitState(GameplayTags.InitState_GameplayReady, false, GameplayTags.InitState_DataInitialized);
	}
}

void UPMGameInstance::Shutdown()
{
	// 에디터에서 플레이를 종료해도 피쳐가 엔진 서브스템이라 비활성화가 안됨.
#ifdef WITH_EDITOR
	TArray<const UGameFeatureData*> ActivePluginFeatureDatas;
	UGameFeaturesSubsystem::Get().GetGameFeatureDataForActivePlugins(ActivePluginFeatureDatas);
	for (const auto& Data : ActivePluginFeatureDatas)
	{
		FString PluginName;
		Data->GetPluginName(PluginName);
		FString PluginURL;
		if (UGameFeaturesSubsystem::Get().GetPluginURLByName(PluginName, PluginURL))
		{
			UGameFeaturesSubsystem::Get().DeactivateGameFeaturePlugin(PluginURL);
		}
	}
#endif
	Super::Shutdown();
}
