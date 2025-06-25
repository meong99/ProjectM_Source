// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpecHandle.h"
#include "ActiveGameplayEffectHandle.h"
#include "Templates/SubclassOf.h"
#include "NativeGameplayTags.h"
#include "MAbilityTypes.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UPMGameplayAbility;
class UPMAbilitySystemComponent;


UE_DECLARE_GAMEPLAY_TAG_EXTERN(Animation_Notify_StartAttack);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Animation_Notify_EndAttack);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Animation_Notify_StartDead);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Animation_Notify_EndDead);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Combat_Hit);


USTRUCT(BlueprintType)
struct FMSetbyCallerFloat 
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag SetByCallerTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Value = 0.0f;
};

USTRUCT(BlueprintType, Blueprintable)
struct FMAbilityInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> EffectClass;
};

// Ability와 Tag를 Wrapping해놓은 구조체
USTRUCT(BlueprintType)
struct FPMAbilitySet_GameplayAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPMGameplayAbility> Ability = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;
};

USTRUCT(BlueprintType)
struct FMAbilitySet_GrantedHandles 
{
	GENERATED_BODY()

	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);

	// 부여된 어빌리티 모두 제거
	void TakeFromAbilitySystem(UPMAbilitySystemComponent* AbilitySystemComp);

protected:
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
};

USTRUCT(BlueprintType)
struct FMAbilitySet_GameplayEffect
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> Effect = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FMSetbyCallerFloat> SetbyCallers;
};

USTRUCT(BlueprintType)
struct FMAbilitySet_AppliedEffectHandles
{
	GENERATED_BODY()

	void AddAppliedEffectHandle(const FActiveGameplayEffectHandle& Handle);

	void RemoveAppliedEffects(UPMAbilitySystemComponent* AbilitySystemComp);

	bool WasApplied() const;

protected:
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> AppliedEffectHandles;
};
