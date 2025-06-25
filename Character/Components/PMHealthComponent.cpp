// Fill out your copyright notice in the Description page of Project Settings.

#include "PMHealthComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/PMAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/PMHealthSet.h"
#include "Character/MCharacterBase.h"
#include "Util/MGameplayStatics.h"
#include "Character/MPlayerCharacterBase.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "PMGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"

UPMHealthComponent::UPMHealthComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	AbilitySystemComponent = nullptr;
	HealthSet = nullptr;
	SetIsReplicatedByDefault(true);
}

void UPMHealthComponent::CallOrRegister_OnInitHealthComponent(FOnInitHealth::FDelegate&& Delegate)
{
	if (HealthSet)
	{
		Delegate.ExecuteIfBound(HealthSet->GetHealthAttribute(), this, 0, HealthSet->GetHealth(), nullptr);
		Delegate.ExecuteIfBound(HealthSet->GetHealthRecoveryAttribute(), this, 0, HealthSet->GetHealthRecovery(), nullptr);
	}
	else
	{
		Delegate_OnInitHealth.Add(MoveTemp(Delegate));
	}
}

void UPMHealthComponent::InitializeWithAbilitySystem(UPMAbilitySystemComponent* InASC)
{
	AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent)
	{
		MCHAE_ERROR("PMHealthComponent: Health component for owner [%s] has already been initialized with an ability system.", *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent)
	{
		MCHAE_ERROR("PMHealthComponent: Cannot initialize health component for owner [%s] with NULL ability system.", *GetNameSafe(Owner));
		return;
	}

	HealthSet = AbilitySystemComponent->GetSet<UPMHealthSet>();
	if (!HealthSet)
	{
		MCHAE_ERROR("PMHealthComponent: Cannot initialize health component for owner [%s] with NULL health set on the ability system.", *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UPMHealthSet::GetHealthAttribute()).AddUObject(this, &ThisClass::HandleHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UPMHealthSet::GetMaxHealthAttribute()).AddUObject(this, &ThisClass::HandleHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UPMHealthSet::GetHealthRecoveryAttribute()).AddUObject(this, &ThisClass::HandleHealthChanged);
	if (HasAuthority())
	{
		AMPlayerCharacterBase* Character = Cast<AMPlayerCharacterBase>(GetOwner());
		if (Character)
		{
			Character->InitCharacterDefaultSpec();
		}
	}

	float CurrentHealth = HealthSet->GetHealth();
	float CurrentHealthRecovery = HealthSet->GetHealthRecovery();
	Delegate_OnInitHealth.Broadcast(HealthSet->GetHealthAttribute(), this, 0, CurrentHealth, nullptr);
	Delegate_OnInitHealth.Broadcast(HealthSet->GetHealthRecoveryAttribute(), this, 0, CurrentHealthRecovery, nullptr);
	Delegate_OnInitHealth.Clear();
}

void UPMHealthComponent::UninitializeWithAbilitySystem()
{
	AbilitySystemComponent = nullptr;
	HealthSet = nullptr;
}

float UPMHealthComponent::GetCurrentHealth()
{
	if (HealthSet)
	{
		return HealthSet->GetHealth();
	}
	
	MCHAE_WARNING("Health is not initialized!");
	return 0;
}

void UPMHealthComponent::HandleHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	float OldValue = ChangeData.OldValue;
	float NewValue = ChangeData.NewValue;
	AActor* Instigator = UPMAbilitySystemComponent::GetInstigatorFromAttrChangeData(ChangeData);
	const FGameplayAttribute& Attribute = ChangeData.Attribute;

	OnChangeHealth(Attribute, OldValue, NewValue, Instigator);
	if (GetNetMode() != NM_Standalone)
	{
		if (IsNetConnectionReady())
		{
			Client_HandleHealthChanged(Attribute, OldValue, NewValue, Instigator);
		}
		else
		{
			GetWorld()->GetTimerManager().SetTimer(Handle, [Attribute, OldValue, NewValue, Instigator, this]()->void
			{
				if (IsNetConnectionReady())
				{
					Client_HandleHealthChanged(Attribute, OldValue, NewValue, Instigator);
					GetWorld()->GetTimerManager().ClearTimer(Handle);
				}
			}, .5f, true, .5f);
		}
	}
}

void UPMHealthComponent::OnChangeHealth(const FGameplayAttribute& Attribute, float OldValue, float NewValue, AActor* Instigator)
{
	OnHealthChanged.Broadcast(Attribute, this, OldValue, NewValue, Instigator);
	if (Attribute == UPMHealthSet::GetHealthAttribute())
	{
		CheckAndNotifyDeath(OldValue, NewValue, Instigator);
	}
}

void UPMHealthComponent::Client_HandleHealthChanged_Implementation(const FGameplayAttribute& Attribute, float OldValue, float NewValue, AActor* Instigator)
{
	OnChangeHealth(Attribute, OldValue, NewValue, Instigator);
}

void UPMHealthComponent::CheckAndNotifyDeath(float OldValue, float NewValue, AActor* Instigator)
{
	if (OldValue > 0 && NewValue <= 0)
	{
		AMCharacterBase* Owner = Cast<AMCharacterBase>(GetOwner());
		if (Owner)
		{
			Owner->OnDead();
		}
	}
}

UPMHealthComponent* UPMHealthComponent::FindHealthComponent(const AActor* Actor)
{
	if (!Actor)
	{
		return nullptr;
	}

	UPMHealthComponent* HealthComponent = Actor->FindComponentByClass<UPMHealthComponent>();
	return HealthComponent;
}

float UPMHealthComponent::GetHealth() const
{
	return (HealthSet ? HealthSet->GetHealth() : 0.0f);
}

float UPMHealthComponent::GetMaxHealth() const
{
	return (HealthSet ? HealthSet->GetMaxHealth() : 0.0f);
}

float UPMHealthComponent::GetHealthNormalized() const
{
	if (HealthSet)
	{
		const float Health = HealthSet->GetHealth();
		const float MaxHealth = HealthSet->GetMaxHealth();
		return ((MaxHealth > 0.0f) ? (Health / MaxHealth) : 0.0f);
	}
	return 0.0f;
}
