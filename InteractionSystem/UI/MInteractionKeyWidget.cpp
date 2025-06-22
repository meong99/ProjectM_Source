#include "MInteractionKeyWidget.h"
#include "Interaction/MInteractionComponent.h"
#include "Interaction/MInteractiveAction_OnInteractionBase.h"

UMInteractionKeyWidget::UMInteractionKeyWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMInteractionKeyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (WidgetInfo.WidgetOwnerActor)
	{
		UMInteractionComponent* InteractionComp = WidgetInfo.WidgetOwnerActor->FindComponentByClass<UMInteractionComponent>();
		if (!InteractionComp)
		{
			SetVisibility(ESlateVisibility::Collapsed);
			return;
		}

		if (InteractionComp->ExistActivatableAction())
		{
			SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
