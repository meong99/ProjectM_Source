#include "MInteractiveAction_ShowWidget.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "UI/MViewportClient.h"
#include "PMGameplayTags.h"
#include "MInteractionComponent.h"
#include "Character/Components/MNameWidgetComponent.h"
#include "UI/Interaction/MInteractionWidgetBase.h"

UE_DISABLE_OPTIMIZATION

UMInteractiveAction_ShowWidget::UMInteractiveAction_ShowWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMInteractiveAction_ShowWidget::InitAction(UMInteractionComponent* InInteractionComponent, AActor* InOwnerActor)
{
	Super::InitAction(InInteractionComponent, InOwnerActor);
}

void UMInteractiveAction_ShowWidget::ActivateAction()
{
	UMViewportClient* ViewportClient = Cast<UMViewportClient>(GetWorld()->GetGameInstance()->GetGameViewportClient());

	if (ViewportClient && InteractionComponent)
	{
		FGameplayTag NameWidgetTag = FPMGameplayTags::Get().UI_Game_NameWidget;
		for (const FGameplayTag& Tag : WidgetTags)
		{
			if (Tag == NameWidgetTag)
			{
				EnableNameWidget();
			}
			else
			{
				ViewportClient->AddWidgetToLayer(Tag, {this, InteractionComponent->GetOwner() });
			}
		}
	}
}

void UMInteractiveAction_ShowWidget::DeactivateAction()
{
	UMViewportClient* ViewportClient = Cast<UMViewportClient>(GetWorld()->GetGameInstance()->GetGameViewportClient());
	if (ViewportClient)
	{
		FGameplayTag NameWidgetTag = FPMGameplayTags::Get().UI_Game_NameWidget;
		for (const FGameplayTag& Tag : WidgetTags)
		{
			if (Tag == NameWidgetTag)
			{
				DisableNameWidget();
			}
			else
			{
				ViewportClient->RemoveWidgetFromLayer(Tag);
			}
		}
	}
}

void UMInteractiveAction_ShowWidget::EnableNameWidget() const
{
	UMNameWidgetComponent* WidgetComp = GetNameWidgetComponent();
	if (WidgetComp)
	{
		WidgetComp->EnableNameWidget();
	}
}

void UMInteractiveAction_ShowWidget::DisableNameWidget() const
{
	UMNameWidgetComponent* WidgetComp = GetNameWidgetComponent();
	if (WidgetComp)
	{
		WidgetComp->DisableNameWidget();
	}
}

UMNameWidgetComponent* UMInteractiveAction_ShowWidget::GetNameWidgetComponent() const
{
	if (InteractionComponent && InteractionComponent->GetOwner())
	{
		return InteractionComponent->GetOwner()->FindComponentByClass<UMNameWidgetComponent>();
	}

	return nullptr;
}

UE_ENABLE_OPTIMIZATION