#include "MWidgetLayer.h"
#include "Components/OverlaySlot.h"

UMWidgetLayer::UMWidgetLayer(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMWidgetLayer::AddWidgetToLayer(UMWidgetBase* Widget)
{
	if (Widget == nullptr || Widget->IsInLayer())
	{
		MCHAE_WARNING("Widget Instance is null or already in layer");
		return;
	}

	UOverlay* LayerAt = GetAppropriateOverlay((int32)Widget->GetWidgetLayerId());
	UOverlaySlot* OverlaySlot = LayerAt ? LayerAt->AddChildToOverlay(Widget) : AddChildToOverlay(Widget);
	if (OverlaySlot)
	{
		OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);

		Widget->SetActivate(true);
		if (Widget->IsFocusable())
		{
			Widget->SetFocus();
		}

		EMWidgetInputMode InputMode = Widget->GetInputMode();
		PushInputMode(InputMode, Widget->GetWidgetTag());
		SetInputMode(Widget);
	}
	else
	{
		MCHAE_WARNING("OverlaySlot is null");
	}
}

void UMWidgetLayer::RemoveWidgetFromLayer(UMWidgetBase* Widget)
{
	if (Widget == nullptr)
	{
		ensure(false);
		MCHAE_WARNING("Widget Instance is null!");
		return;
	}

	int32 WidgetLayerId = (int32)Widget->GetWidgetLayerId();
	UOverlay* LayerAt = GetAppropriateOverlay(WidgetLayerId);
	if (!LayerAt)
	{
		// Layer가 비정상임
		ensure(false);
		return;
	}

	LayerAt->RemoveChild(Widget);
	Widget->SetActivate(false);
	EMWidgetInputMode InputMode = Widget->GetInputMode();
	PopInputMode(InputMode);
	SetInputMode();
}

void UMWidgetLayer::ActivateLayer()
{
	APlayerController* PlayerController = GetOwningPlayer();
	if (PlayerController)
	{
		FInputModeGameAndUI NewMode;
		NewMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		PlayerController->SetInputMode(NewMode);
		PlayerController->bShowMouseCursor = true;
		OldTopInputMode = CurrentTopInputMode;
		CurrentTopInputMode = GetTopInputMode();
	}
}

void UMWidgetLayer::ClearAllWidget()
{
	for (int32 i = 0; i < (int32)EMWidgetLayerId::None; i++)
	{
		UOverlay* LayerAt = GetAppropriateOverlay(i);

		if (LayerAt)
		{
			LayerAt->ClearChildren();
		}
	}
}

bool UMWidgetLayer::RemoveTopWidgetInGameLayer()
{
	UOverlay* LayerAt = GetAppropriateOverlay((int32)EMWidgetLayerId::Game);
	if (LayerAt)
	{
		TArray<UWidget*> Children = LayerAt->GetAllChildren();
		UMWidgetBase* Widget = Children.Num() > 0 ? Cast<UMWidgetBase>(Children.Last()) : nullptr;
		if (Widget)
		{
			RemoveWidgetFromLayer(Widget);

			return true;
		}
	}

	return false;
}

UOverlay* UMWidgetLayer::GetAppropriateOverlay(const int32 Id)
{
	UOverlay* LayerAt = Cast<UOverlay>(GetChildAt(Id));
	return LayerAt ? LayerAt : this;
}

void UMWidgetLayer::SetInputMode(UMWidgetBase* Widget) const
{
	APlayerController* PlayerController = GetOwningPlayer();
	if (PlayerController == nullptr)
	{
		return;
	}

	if (OldTopInputMode == CurrentTopInputMode)
	{
		return;
	}

	switch (CurrentTopInputMode)
	{
		case EMWidgetInputMode::GameOnly:
		{
			FInputModeGameOnly NewMode;
			PlayerController->bShowMouseCursor = false;
			PlayerController->SetInputMode(NewMode);
			break;
		}
		case EMWidgetInputMode::GameAndUIWithShowMouse:
		{
			FInputModeGameAndUI NewMode;
			NewMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
			PlayerController->SetInputMode(NewMode);
			PlayerController->bShowMouseCursor = true;
			break;
		}
		case EMWidgetInputMode::UIOnly:
		{
			FInputModeUIOnly NewMode;
			NewMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
			PlayerController->SetInputMode(NewMode);
			PlayerController->FlushPressedKeys();
			PlayerController->bShowMouseCursor = true;
			break;
		}
		default:
		{
			break;
		}
	}
}

void UMWidgetLayer::PushInputMode(EMWidgetInputMode Input, const FGameplayTag& Tag)
{
	if (Tag.IsValid())
	{
		TStack<FGameplayTag>& WidgetStack = WidgetMapByInputMode.FindOrAdd(Input);
		WidgetStack.Push(Tag);

		if (CurrentTopInputMode < Input)
		{
			OldTopInputMode = CurrentTopInputMode;
			CurrentTopInputMode = Input;
		}
	}
}

void UMWidgetLayer::PopInputMode(EMWidgetInputMode Input)
{
	TStack<FGameplayTag>& WidgetStack = WidgetMapByInputMode.FindOrAdd(Input);
	if (!WidgetStack.IsEmpty())
	{
		WidgetStack.Pop();
		OldTopInputMode = CurrentTopInputMode;
		CurrentTopInputMode = GetTopInputMode();
	}
}

EMWidgetInputMode UMWidgetLayer::GetTopInputMode(EMWidgetInputMode Input) const
{
	EMWidgetInputMode InputMode = Input;

	if (!WidgetMapByInputMode.FindRef(EMWidgetInputMode::GameAndUIWithShowMouse).IsEmpty())
	{
		InputMode = EMWidgetInputMode::GameAndUIWithShowMouse;
	}

	if (!WidgetMapByInputMode.FindRef(EMWidgetInputMode::UIOnly).IsEmpty())
	{
		InputMode = EMWidgetInputMode::UIOnly;
	}

	return InputMode;
}
