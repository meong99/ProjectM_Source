#pragma once

#include "Input/PMMappableConfigPair.h"
#include "GameFeaturesSubsystem.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFeatureAction_WorldActionBase.h"

#include "GameFeatureAction_AddInputConfig.generated.h"

class AActor;

UCLASS()
class PROJECTM_API UGameFeatureAction_AddInputConfig : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

	// Engine Subsystem관련 문제로 PIE상황일 때 InputConfig를 적용시킨 Pawn을 관리하기 위해서 Pawn과 Handle을 들고있는다.
	// 만약 정상적이지 않은 pawn이거나, 제거해야하는 상황일때마다 제거하며 관리한다.
	struct FPerContextData
	{
		// Event발생 관련 Handle.
		TArray<TSharedPtr<FComponentRequestHandle>> ExtensionRequestHandles;

		// 위 이벤트로 인해 적용된 Pawn
		TArray<TWeakObjectPtr<APawn>> PawnsAddedTo;
	};

/*
* Overrided Function
*/
public:
	UGameFeatureAction_AddInputConfig();

	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;

	// Pawn에 대해서 FrameWorkComponentManager에서 Event를 호출했을 때 실행할 Delegate를 Binding한다.
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;

private:
	// Event호출 콜백 함수. 여기에서 본격적으로 Action을 적용시킨다.
	void HandlePawnExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);

	void AddInputConfig(APawn* Pawn, FPerContextData& ActiveData);
	void RemoveInputConfig(APawn* Pawn, FPerContextData& ActiveData);

	void Reset(FPerContextData& ActiveData);

/*
* Member Variables
*/
private:

	UPROPERTY(EditAnywhere)
	TArray<FPMMappableConfigPair> InputConfigs;

	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;
};