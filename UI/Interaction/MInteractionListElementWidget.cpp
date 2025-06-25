#include "MInteractionListElementWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Interaction/MInteractiveAction_OnInteractionBase.h"

UMInteractionListElementWidget::UMInteractionListElementWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

FReply UMInteractionListElementWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InteractiveAction)
	{
		InteractiveAction->ActivateAction();
	}

	return FReply::Handled();
}

void UMInteractionListElementWidget::SetInteractiveAction(UMInteractiveAction_OnInteractionBase* InInteractiveAction)
{
	InteractiveAction = InInteractiveAction;
	if (InteractiveAction)
	{
		SetActionName(InteractiveAction->GetActionName());
	}
}

void UMInteractionListElementWidget::SetActionName(const FText& InActionName)
{
	ActionName->SetText(InActionName);
}

void UMInteractionListElementWidget::SetBackgroundImage(UTexture2D* InBackgroundImage)
{
	BackgroundImage->SetBrushFromTexture(InBackgroundImage);
}
