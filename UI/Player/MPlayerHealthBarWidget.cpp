#include "MPlayerHealthBarWidget.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Character/Components/PMHealthComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "AbilitySystem/Attributes/PMHealthSet.h"

UMPlayerHealthBarWidget::UMPlayerHealthBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMPlayerHealthBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	APlayerController* Controller = GetOwningPlayer();
	if (Controller)
	{
		Controller->OnPossessedPawnChanged.AddDynamic(this, &UMPlayerHealthBarWidget::Callback_OnPossessedPawnChanged);
		Callback_OnPossessedPawnChanged(nullptr, Controller->GetPawn());
	}
}

void UMPlayerHealthBarWidget::Callback_OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	if (IsValid(OldPawn))
	{
		UPMHealthComponent* HealthComp = OldPawn->FindComponentByClass<UPMHealthComponent>();
		if (HealthComp)
		{
			HealthComp->OnHealthChanged.RemoveDynamic(this, &UMPlayerHealthBarWidget::Callback_OnHealthChanged);
		}
	}

	if (IsValid(NewPawn))
	{
		UPMHealthComponent* HealthComp = NewPawn->FindComponentByClass<UPMHealthComponent>();
		if (HealthComp)
		{
			HealthComp->CallOrRegister_OnInitHealthComponent(FOnInitHealth::FDelegate::CreateUObject(this, &UMPlayerHealthBarWidget::Callback_OnHealthChanged));
			HealthComp->OnHealthChanged.AddDynamic(this, &UMPlayerHealthBarWidget::Callback_OnHealthChanged);
		}
	}
}

void UMPlayerHealthBarWidget::Callback_OnHealthChanged(const FGameplayAttribute& Attribute, UPMHealthComponent* HealthComp, const float OldValue, const float NewValue, AActor* Instigator)
{
	if (Attribute == UPMHealthSet::GetHealthRecoveryAttribute())
	{
		HealthRecoveryText->SetText(FText::AsNumber(NewValue));
	}
	else if (HealthComp)
	{
		const float MaxHealth = Attribute == UPMHealthSet::GetMaxHealthAttribute() ? NewValue : HealthComp->GetMaxHealth();
		const float CurrentHealth = Attribute == UPMHealthSet::GetMaxHealthAttribute() ? HealthComp->GetCurrentHealth() : NewValue;
		const float Percent = CurrentHealth / MaxHealth;

		FText Template = MakeFormatText(CurrentHealth, MaxHealth);

		ProgressBar->SetPercent(Percent);
		HealthText->SetText(Template);
	}
}

FText UMPlayerHealthBarWidget::MakeFormatText(const float Health, const float MaxHealth) const
{
	FText Template = FText::FromString("{Health} / {MaxHealth}");
	FFormatNamedArguments Args;

	Args.Add("Health", FText::AsNumber((int32)Health));
	Args.Add("MaxHealth", FText::AsNumber((int32)MaxHealth));

	return FText::Format(Template, Args);
}
