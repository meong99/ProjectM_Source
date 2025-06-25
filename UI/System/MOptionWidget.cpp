#include "MOptionWidget.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

UMOptionWidget::UMOptionWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMOptionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	BindDelegates();
}

void UMOptionWidget::BindDelegates()
{
	ExitButton->OnClicked.AddDynamic(this, &UMOptionWidget::OnClick_ExitButton);
	LobbyButton->OnClicked.AddDynamic(this, &UMOptionWidget::OnClick_LobbyButton);
}

void UMOptionWidget::OnClick_ExitButton()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UMOptionWidget::OnClick_LobbyButton()
{
}
