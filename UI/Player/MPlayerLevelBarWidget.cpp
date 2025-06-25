#include "MPlayerLevelBarWidget.h"
#include "Components/TextBlock.h"
#include "Player/PMPlayerState.h"
#include "Components/MLevelComponent.h"
#include "Components/ProgressBar.h"
#include "Player/PMPlayerControllerBase.h"

UMPlayerLevelBarWidget::UMPlayerLevelBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMPlayerLevelBarWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	APMPlayerControllerBase* Controller = Cast<APMPlayerControllerBase>(GetOwningPlayer());
	if (Controller)
	{
		Controller->CallOrRegister_OnReadyPlayerState(FOnReadyPlayState::FDelegate::CreateUObject(this, &UMPlayerLevelBarWidget::Callback_OnReadyPlayerState));
	}
}

void UMPlayerLevelBarWidget::Callback_OnReadyPlayerState(APlayerState* PlayerState)
{
	UMLevelComponent* LevelComponent = PlayerState ? PlayerState->FindComponentByClass<UMLevelComponent>() : nullptr;
	if (LevelComponent)
	{
		if (Handle_ChangeLevel.IsValid())
		{
			LevelComponent->RemoveDelegate_OnChangeLevel(Handle_ChangeLevel);
			Handle_ChangeLevel.Reset();
		}
		if (Handle_ChangeExp.IsValid())
		{
			LevelComponent->RemoveDelegate_OnChangeExp(Handle_ChangeExp);
			Handle_ChangeExp.Reset();
		}
		Handle_ChangeLevel = LevelComponent->CallAndRegister_OnChangeLevel(FOnChange_Level::FDelegate::CreateUObject(this, &UMPlayerLevelBarWidget::OnChange_Level));
		Handle_ChangeExp = LevelComponent->CallAndRegister_OnChangeExp(FOnChange_Exp::FDelegate::CreateUObject(this, &UMPlayerLevelBarWidget::OnChange_Exp));
	}
}

void UMPlayerLevelBarWidget::OnChange_Level(const int32 OldLevel, const int32 InCurrentLevel, const int64 InMaxExp)
{
	CurrentLevel = InCurrentLevel;
	MaxExp = InMaxExp;

	CurrentLevelText->SetText(FText::AsNumber(CurrentLevel));
	MaxExpText->SetText(FText::AsNumber(MaxExp));
}

void UMPlayerLevelBarWidget::OnChange_Exp(const int64 OldExp, const int64 InCurrentExp)
{
	CurrentExp = InCurrentExp;
	CurrentExpText->SetText(FText::AsNumber(CurrentExp));

	float Percent = MaxExp > 0 ? (double)CurrentExp / MaxExp : 0.f;
	LevelBar->SetPercent(Percent);
}
