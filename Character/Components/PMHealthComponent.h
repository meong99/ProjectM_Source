// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "Components/MPawnComponentBase.h"
#include "AttributeSet.h"
#include "PMHealthComponent.generated.h"

class UPMAbilitySystemComponent;
class UPMHealthSet;
class UPMHealthComponent;
class AActor;
struct FOnAttributeChangeData;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FPMHealth_AttributeChanged, const FGameplayAttribute&, Attribute, UPMHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);
DECLARE_MULTICAST_DELEGATE_FiveParams(FOnInitHealth, const FGameplayAttribute& Attribute, UPMHealthComponent* HealthComponent, float OldValue, float NewValue, AActor* Instigator);


UCLASS()
class PROJECTM_API UPMHealthComponent : public UMPawnComponentBase
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UPMHealthComponent(const FObjectInitializer& ObjectInitializer);

/*
* Member Functions
*/
public:
	void CallOrRegister_OnInitHealthComponent(FOnInitHealth::FDelegate&& Delegate);

	UFUNCTION(BlueprintPure, Category = "ProjectM | Health")
	static UPMHealthComponent* FindHealthComponent(const AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "ProjectM | Health")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "ProjectM | Health")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "ProjectM | Health")
	float GetHealthNormalized() const;

	void InitializeWithAbilitySystem(UPMAbilitySystemComponent* InASC);
	void UninitializeWithAbilitySystem();

	float	GetCurrentHealth();

private:
	UFUNCTION(Client, Reliable)
	void	Client_HandleHealthChanged(const FGameplayAttribute& Attribute, float OldValue, float NewValue, AActor* Instigator);

	void	HandleHealthChanged(const FOnAttributeChangeData& ChangeData);
	void	OnChangeHealth(const FGameplayAttribute& Attribute, float OldValue, float NewValue, AActor* Instigator);
	void	CheckAndNotifyDeath(float OldValue, float NewValue, AActor* Instigator);

/*
* Member Variables
*/
public:
	UPROPERTY(BlueprintAssignable)
	FPMHealth_AttributeChanged OnHealthChanged;

private:
	FOnInitHealth Delegate_OnInitHealth;

	UPROPERTY()
	TObjectPtr<UPMAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<const UPMHealthSet> HealthSet;

	FTimerHandle Handle;
};
