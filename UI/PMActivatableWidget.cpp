#include "PMActivatableWidget.h"

UPMActivatableWidget::UPMActivatableWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{}

TOptional<FUIInputConfig> UPMActivatableWidget::GetDesiredInputConfig() const
{
	switch (InputConfig)
	{
	case EPMWidgetInputMode::GameAndMenu:
		return FUIInputConfig(ECommonInputMode::All, GameMouseCaptureMode);
	case EPMWidgetInputMode::Game:
		return FUIInputConfig(ECommonInputMode::Game, GameMouseCaptureMode);
	case EPMWidgetInputMode::Menu:
		return FUIInputConfig(ECommonInputMode::Menu, GameMouseCaptureMode);
	case EPMWidgetInputMode::Default:
	default:
		return TOptional<FUIInputConfig>();
	}
}
