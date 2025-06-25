// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilities/Public/ActiveGameplayEffectHandle.h"
#include "GameplayTagContainer.h"
#include "PMAbilitySystemComponent.generated.h"

UCLASS()
class PROJECTM_API UPMAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UPMAbilitySystemComponent();
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

protected:

/*
* Member Functions
*/
public:
	static AActor* GetInstigatorFromAttrChangeData(const FOnAttributeChangeData& ChangeData);

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);

	FGameplayEffectSpec				MakeGameplayEffectSpecWithSetByCaller(const FGameplayEffectContextHandle& ContextHandle, 
																		  TSubclassOf<UGameplayEffect> EffectClass, TMap<FGameplayTag, float> SetbyCallerMap, const FGameplayTag& DynamicTag = FGameplayTag::EmptyTag);
	FGameplayEffectContextHandle	MakeGameplayEffectContext(AActor* Instigator, AActor* EffectCauser, const FHitResult& HitResult = {});

	UFUNCTION(BlueprintCallable)
	void SendGameplayTagToAbility(const FGameplayTag& InputTag, const FGameplayTag& SendTag);

/*
* Member Variables
*/
private:
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputHeldSpenHandles;
};