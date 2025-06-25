#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "PMInputConfig.generated.h"

class UInputAction;

USTRUCT(BlueprintType)
struct FPMInputAction 
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag,UI.Registry"))
	FGameplayTag InputTag;
};

UCLASS()
class PROJECTM_API UPMInputConfig : public UDataAsset
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UPMInputConfig(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

/*
* Member Functions
*/
public:
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;
	const UInputAction* FindToggleInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

/*
* Member Variables
*/
public:
	// 직접 바인딩 해야하는 액션 (Vector, float ... 등등 다양한 데이터를 받는 액션)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FPMInputAction> NativeInputActions;

	// 입력 시 어빌리티를 사용하는 액션
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FPMInputAction> AbilityInputActions;

	// 기타 boolean값을 입력으로 토글링하는 액션
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FPMInputAction> ToggleInputActions;
};