#include "MInteractiveAction_OnInteractionBase.h"
#include "MInteractionComponent.h"

UMInteractiveAction_OnInteractionBase::UMInteractiveAction_OnInteractionBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMInteractiveAction_OnInteractionBase::ActivateAction()
{
	Super::ActivateAction();

	if (InteractionComponent)
	{
		InteractionComponent->OnActivatedInteractiveAction(this);
	}
}

void UMInteractiveAction_OnInteractionBase::DeactivateAction()
{
	if (InteractionComponent)
	{
		InteractionComponent->OnDeactivatedInteractiveAction(this);
	}
	Super::DeactivateAction();
}
