#include "PMHealthSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "Util/MGameplayStatics.h"
#include "Character/MCharacterBase.h"

UPMHealthSet::UPMHealthSet() : Super(), Health(1.f), MaxHealth(1.f), HealthRecovery(0.f)
{
}

void UPMHealthSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPMHealthSet, Health);
	DOREPLIFETIME(UPMHealthSet, MaxHealth);
	DOREPLIFETIME(UPMHealthSet, HealthRecovery);
}

void UPMHealthSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	ClampToZeroIfDead(NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UPMHealthSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	ClampToZeroIfDead(NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UPMHealthSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(GetHealth(), OldValue, NewValue, GetHealthAttribute());
	}
}

void UPMHealthSet::AdjustAttributeForMaxChange(const float CurrentValue, const float OldMaxValue, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent*	AbilityComp = GetOwningAbilitySystemComponent();

	if (!FMath::IsNearlyEqual(OldMaxValue, NewMaxValue) && CurrentValue > 0.f && IsValid(AbilityComp))
	{
		float	NewDelta = (OldMaxValue > 0.f) ? (CurrentValue * NewMaxValue / OldMaxValue) - CurrentValue : NewMaxValue;
		if (!FMath::IsNearlyEqual(NewDelta, 0.f))
		{
			AbilityComp->ApplyModToAttribute(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
		}
	}
	else if (FMath::IsNearlyEqual(CurrentValue, 0))
	{
		AbilityComp->ApplyModToAttribute(AffectedAttributeProperty, EGameplayModOp::Override, NewMaxValue);
	}
}

void UPMHealthSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
}

void UPMHealthSet::ClampToZeroIfDead(float& Value) const
{
	if (GetActorInfo())
	{
		AMCharacterBase* Character = Cast<AMCharacterBase>(GetActorInfo()->AvatarActor);
		if (Character && Character->GetCharacterLifeState() > EMCharacterLiftState::Alive)
		{
			Value = 0;
		}
	}
}
