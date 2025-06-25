#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncAction_ExperienceReady.generated.h"

class UCommonSession_HostSessionRequest;
class AGameStateBase;
class UPMExperienceDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FExperienceReadyAsyncDelegate);

UCLASS()
class PROJECTM_API UAsyncAction_ExperienceReady : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UAsyncAction_ExperienceReady();

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static UAsyncAction_ExperienceReady* WaitForExperienceReady(UObject* WorldContextObject);

	virtual void Activate() override;

	void Step1_HandleGameStateSet(AGameStateBase* GameState);
	void Step2_ListenToExperienceLoading(AGameStateBase* GameState);
	void Step3_HandleExperienceLoaded(const UPMExperienceDefinition* CurrentExperience);
	void Step4_BroadcastReady();

	UPROPERTY(BlueprintAssignable)
	FExperienceReadyAsyncDelegate OnReady;

	TWeakObjectPtr<UWorld> WorldPtr;
};