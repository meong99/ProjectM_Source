#pragma once

#include "Components/GameFrameworkInitStateInterface.h"
#include "Input/PMMappableConfigPair.h"
#include "Components/MPawnComponentBase.h"
#include "PMCharacterInitComponent.generated.h"

class UPMCameraMode;
struct FInputActionValue;
class UPMInputComponent;

/*
* 캐릭터와 관련된 시스템의 초기화를 담당하는 컴포넌트이다.
* 기존의 캐릭터에서 사용하는 Input이라던가, 카메라 등 캐릭터에 직접 부착하면 캐릭터에 종속되어버려 다른 곳에 붙이기 힘드니
* 컴포넌트로 빼서 필요한 곳에 부착하는 형태로 간다.
* 
*/
// @Warning PawnExtensionComponent의 하위 컴포넌트는 무조건 BP에서 추가되어야한다! 그렇지 않으면 초기화 순서가 보장되지 않는다!
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class PROJECTM_API UPMCharacterInitComponent : public UMPawnComponentBase, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UPMCharacterInitComponent(const FObjectInitializer& ObjectInitializer);

	virtual void OnRegister() final;
	virtual void BeginPlay() final;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) final;

	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) final;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const final;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) final;
	virtual void CheckDefaultInitialization() final;

	virtual FName GetFeatureName() const final { return NAME_ActorFeatureName; }
/*
* Member Functions
*/
public:
	TSubclassOf<UPMCameraMode> DetermineCameraMode() const;
	void InitializePlayerInput(UInputComponent* PlayerInputComponent);

protected:
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Jump(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);
	void Input_ToggleInputTag(FGameplayTag InputTag);
/*
* Member Variables
*/
public:
	static const FName NAME_ActorFeatureName;
	static const FName NAME_BindInputsNow;

private:
	// 입력에 대한 단위
	UPROPERTY(EditAnywhere)
	TArray<FPMMappableConfigPair> DefaultInputConfigs;

	UPROPERTY()
	UPMInputComponent* InputComponent;
};