// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "PMAttributeSet.h"
#include "PMCombatSet.generated.h"

UCLASS()
class PROJECTM_API UPMCombatSet : public UPMAttributeSet
{
	GENERATED_BODY()
public:
	UPMCombatSet();

	ATTRIBUTE_ACCESSORS(UPMCombatSet, DefensePower);
	ATTRIBUTE_ACCESSORS(UPMCombatSet, AttackPower);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "ProjectM | Combat")
	FGameplayAttributeData DefensePower;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "ProjectM | Combat")
	FGameplayAttributeData AttackPower;
};
