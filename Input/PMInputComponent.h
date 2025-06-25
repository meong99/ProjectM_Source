#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "PMInputConfig.h"
#include "GameplayTagContainer.h"
#include "PMInputComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FInputActionDelegate, const FGameplayTag&);

UCLASS()
class PROJECTM_API UPMInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

	/*
	* Overrided Function
	*/
public:
	UPMInputComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/*
	* Member Functions
	*/
	template<class UserClass, typename FuncType>
	void BindNativeAction(const UPMInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound);

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UPMInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);

	template<class UserClass, typename ToggleFuncType>
	void BindToggleActions(const UPMInputConfig* InputConfig, UserClass* Object, ToggleFuncType ToggleFunc, TArray<uint32>& BindHandles);

	/*
	* Member Variables
	*/
public:
	// 입력에 따른 동작을 담아놓은 델리게이트 맵. Tag와 입력에 따른 동작이 1:1매핑된다.
	TMap<FGameplayTag, FInputActionDelegate::FDelegate> Delegate_InputActionMap;
};

template<class UserClass, typename FuncType>
void UPMInputComponent::BindNativeAction(const UPMInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputConfig);

	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound))
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
	else
	{
		MCHAE_WARNING("Native 바인딩 실패함!! 태그 확인해야함!");
	}
}

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UPMInputComponent::BindAbilityActions(const UPMInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
{
	check(InputConfig);

	for (const FPMInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag).GetHandle());
			}

			if (ReleasedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
			}
		}
	}
}

template<class UserClass, typename ToggleFuncType>
void UPMInputComponent::BindToggleActions(const UPMInputConfig* InputConfig, UserClass* Object, ToggleFuncType ToggleFunc, TArray<uint32>& BindHandles)
{
	check(InputConfig);

	for (const FPMInputAction& Action : InputConfig->ToggleInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (ToggleFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ToggleFunc, Action.InputTag).GetHandle());
			}
		}
	}
}