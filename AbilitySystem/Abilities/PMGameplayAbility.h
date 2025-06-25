// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "NativeGameplayTags.h"
#include "Types/MAbilityTypes.h"
#include "PMGameplayAbility.generated.h"

class UPMAbilityCost;

UENUM(BlueprintType)
enum class EPMAbilityActivationPolicy : uint8
{
	// Input이 Trigger되었을 때(Preddes/Released)
	OnInputTriggered,
	// Input이 Held되었을 때
	WhileInputActive,
	//Avatar가 생성되었을 때 즉시 적용
	OnSpawn,
};

UCLASS()
class PROJECTM_API UPMGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UPMGameplayAbility();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags /* = nullptr */) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

protected:

/*
* Member Functions
*/
public:
	virtual void	OnReceivedGameplayTag(const FGameplayTag& Tag) {}

	bool						CanRetrigger() const { return bRetriggerInstancedAbility; }
	EPMAbilityActivationPolicy	GetActivationPolicy() const { return ActivationPolicy; }
/*
* Member Variables
*/
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectM | AbilityActivation")
	EPMAbilityActivationPolicy ActivationPolicy;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "ProjectM | Costs")
	TArray<TObjectPtr<UPMAbilityCost>> AdditionalCosts;
};