// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/PMGameplayAbility.h"
#include "MAbility_MonsterAttackBase.generated.h"

class UMMonsterDefinition;
class AActor;

UCLASS()
class PROJECTM_API UMAbility_MonsterAttackBase : public UPMGameplayAbility
{
	GENERATED_BODY()

	/*
	* Overrided Functions
	*/
public:
	UMAbility_MonsterAttackBase();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

/*
* Member Functions
*/
public:

protected:
	virtual void TraceAttack();

	UFUNCTION()
	void CallBack_MontageEnded();

	UFUNCTION()
	void Callback_AttackPoint(FGameplayEventData Payload);
/*
* Member Variables
*/
protected:
	UPROPERTY();
	UMMonsterDefinition* MonsterDef;
};