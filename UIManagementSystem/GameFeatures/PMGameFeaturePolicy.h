#pragma once

#include "GameFeaturesProjectPolicies.h"
#include "GameFeatureStateChangeObserver.h"
#include "PMGameFeaturePolicy.generated.h"

/*
* GameFeature Plugin이 Memory에 로딩되거나, Active를 관리하는 StateMachine을 모니터링

* 기본적으로 GameFeatureManager를 상속받아서 어떤 규칙을 정의하는건 너무 복잡하기 때문에 Unreal에서 제공하는 UDefaultGameFeaturesProjectPolicies를 상속받아서
* 우리가 원하는 GameFeature의 규칙을 정하는 것이다.
* 기본적으로 BaseClass는 비어있기에 해당 클래스를 확인해서 overriding을 통해 규칙을 정하자
*/
UCLASS()
class PROJECTM_API UPMGameFeaturePolicy : public UDefaultGameFeaturesProjectPolicies
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UPMGameFeaturePolicy();

	// GameFeature가 초기화되는 단계
	virtual void InitGameFeatureManager() override;
	// GameFeature가 종료되는 단계
	virtual void ShutdownGameFeatureManager() override;

		/*
* Member Functions
*/
public:

/*
* Member Variables
*/
public:
	// Observer로서 등록한 객체를 관리
	UPROPERTY(Transient)
	TArray<TObjectPtr<UObject>> Observers;
};




/*
* GameFeature Plugin이 Register/Unregister 단계에서 GameplayCuePath가 등록되어 GameplayCueManager가 관련 GameplayCue 에셋을 스캔할 수 있도록
* 이를 위해 GameFeatureStateChangeObserver를 활용해 옵저버 패턴으로 이를 가능하게 하며 이를 위해선 결국 GameFeature System에 등록하는 추가적인 구현이 필요한데 이는 Policy를 이용한다.
* 
* 참고로 GameFeatureAction에도 Registering/Unregistering이 있지만 그걸 호출해줘야 하는건 동일하기에 직접 구현한다.
*/
UCLASS()
class UPMGameFeature_AddGameplayCuePaths : public UObject, public IGameFeatureStateChangeObserver
{
	GENERATED_BODY()
/*
* Overrided Functions
*/
public:
	virtual void OnGameFeatureRegistering(const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL) override;
	virtual void OnGameFeatureUnregistering(const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL) override;

/*
* Member Functions
*/
public:

/*
* Member Variables
*/
public:
};