#include "MInteractionListWidget.h"
#include "Interaction/MInteractionComponent.h"
#include "MInteractionListElementWidget.h"
#include "Interaction/MInteractiveAction_OnInteractionBase.h"
#include "Components/VerticalBox.h"

UMInteractionListWidget::UMInteractionListWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMInteractionListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InteractionComponent = WidgetInfo.WidgetOwnerActor ? WidgetInfo.WidgetOwnerActor->FindComponentByClass<UMInteractionComponent>() : nullptr;
	if (InteractionComponent)
	{
		const TArray<UMInteractiveAction_OnInteractionBase*>& Actions = InteractionComponent->GetOnInteractActions();
		for (int32 i = 0; i < Actions.Num(); i++)
		{
			if (Actions[i] && Actions[i]->ShouldActivate())
			{
				UMInteractionListElementWidget* NewElement = CreateWidget<UMInteractionListElementWidget>(GetOwningPlayer(), ElementWidgetClass);
				if (NewElement)
				{
					NewElement->SetInteractiveAction(Actions[i]);
					InteractionList->AddChildToVerticalBox(NewElement);
				}
			}
		}

		InteractionComponent->OnAddedInteractionList();
	}
}

void UMInteractionListWidget::NativeDestruct()
{
	InteractionList->ClearChildren();
	DeactivateList();
	Super::NativeDestruct();
}

FReply UMInteractionListWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	DeactivateList();

	return FReply::Handled();
}

void UMInteractionListWidget::DeactivateList()
{
	if (InteractionComponent)
	{
		InteractionComponent->OnRemovedInteractionList();
	}
	RemoveWidgetFromLayer();
}
