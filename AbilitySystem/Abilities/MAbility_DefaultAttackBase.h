// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PMGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "MAbility_DefaultAttackBase.generated.h"

class ACharacter;
class AActor;
class UMWeaponItemDefinition;
class UPMWeaponInstance;
class UAbilityTask;
class UPMAbilitySystemComponent;
struct FGameplayEffectSpec;

UCLASS()
class PROJECTM_API UMAbility_DefaultAttackBase : public UPMGameplayAbility
{
	GENERATED_BODY()

	/*
	* Overrided Functions
	*/
public:
	UMAbility_DefaultAttackBase();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

/*
* Member Functions
*/
public:

protected:
	UFUNCTION()
	virtual void StartAttackTracing(FGameplayEventData Payload);
	UFUNCTION()
	virtual void EndAttackTracing(FGameplayEventData Payload);
	UFUNCTION()
	void NotifyMontageEndCallBack();

	bool PlayMontage();
	bool BindAnimNotify();
	bool RotateToTarget(AActor* Target);

	AActor* FindNearlestTarget() const;

	virtual void TraceAttack(ACharacter* OwnerCharacter);
	virtual void Callback_OnHit(const TArray<FHitResult>& HitResults);
	virtual void ApplyEffectToTarget(UPMAbilitySystemComponent* OwnerAbilitySystem, const FHitResult& HitResult);

/*
* Member Variables
*/
protected:
	UPROPERTY();
	UMWeaponItemDefinition* ItemDef;

	UPROPERTY()
	TSet<AActor*> OverlappedActors;

	UPROPERTY()
	TObjectPtr<UAbilityTask> TraceTask;

	TWeakObjectPtr<AActor> WeakWeaponActor;

	int32 MontageIndex = 0;

	UPROPERTY(Replicated)
	bool bCanCombo = true;
};