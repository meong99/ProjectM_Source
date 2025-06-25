// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Types/MAbilityTypes.h"
#include "MGameplayEffectSet.generated.h"

class UPMAbilitySystemComponent;
class AActor;

/**
 *
 */
UCLASS(EditInlineNew, DefaultToInstanced)
class PROJECTM_API UMGameplayEffectSet : public UPrimaryDataAsset
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UMGameplayEffectSet();

/*
* Member Functions
*/
public:
	void ApplyGameplayEffectsToAsc(UPMAbilitySystemComponent* AbilitySystemComp, OUT FMAbilitySet_AppliedEffectHandles* OutAppliedEffectHandles, AActor* Instigator, AActor* EffectCausor, UObject* SourceObject) const;
	void ApplyGameplayEffectsToAsc(UPMAbilitySystemComponent* AbilitySystemComp, 
								   OUT FMAbilitySet_AppliedEffectHandles* OutAppliedEffectHandles, 
								   TFunctionRef<float (const FGameplayTag& SetTag, const float DefaultValue)> Calculate,
								   AActor* Instigator, 
								   AActor* EffectCausor,
								   UObject* SourceObject) const;

	const TArray<FMAbilitySet_GameplayEffect>& GetEffectToApply() const { return EffectToApply; }
/*
* Member Variables
*/
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess), Category = "GameplayEffects")
	TArray<FMAbilitySet_GameplayEffect> EffectToApply;
};
