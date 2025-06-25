#include "MWidgetLayout.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "GameFramework/PlayerController.h"
#include "MWidgetBase.h"
#include "MWidgetLayer.h"
#include "Blueprint/WidgetTree.h"

UMWidgetLayout::UMWidgetLayout(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{}

void UMWidgetLayout::PreAddToLayer()
{
	Super::PreAddToLayer();

	MakeLayerById(GameLayer);
	MakeLayerById(IndependentLayer);
}

void UMWidgetLayout::NativeConstruct()
{
	Super::NativeConstruct();
	UMWidgetLayer* CurrentLayer = GetCurrentLayer();
	if (CurrentLayer)
	{
		CurrentLayer->ActivateLayer();
	}
}

FReply UMWidgetLayout::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	return FReply::Unhandled();
}

FReply UMWidgetLayout::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);

	return FReply::Unhandled();
}

void UMWidgetLayout::MakeLayerById(UMWidgetLayer* MainLayer)
{
	if (!MainLayer)
	{
		ensure(false);
		return;
	}

	for (int32 i = 0; i < (int32)EMWidgetLayerId::None; i++)
	{
		UOverlay* Overlay = WidgetTree->ConstructWidget<UOverlay>();
		UOverlaySlot* OverlaySlot = MainLayer->AddChildToOverlay(Overlay);
		if (OverlaySlot)
		{
			OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
			OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
		}
	}
}

void UMWidgetLayout::ChangeWidgetLayer(EMWidgetLayout NewWidgetLayout) const
{
	LayoutSwitcher->SetActiveWidgetIndex((int32)NewWidgetLayout);
	UMWidgetLayer* Layer = GetLayer(NewWidgetLayout);
	if (Layer)
	{
		Layer->ActivateLayer();
	}
}

void UMWidgetLayout::ClearAllWidget()
{
	for (int32 i = 0; i < (int32)EMWidgetLayout::None; i++)
	{
		UMWidgetLayer* Layer = GetLayer((EMWidgetLayout)i);
		if (Layer)
		{
			Layer->ClearAllWidget();
		}
	}
}

void UMWidgetLayout::AddWidgetToCurrentLayer(UMWidgetBase* Widget) const
{
	if (Widget)
	{
		AddWidgetToLayer(Widget);
	}
}

void UMWidgetLayout::RemoveWidgetToCurrentLayer(UMWidgetBase* Widget) const
{
	RemoveWidgetFromLayer(Widget);
}

void UMWidgetLayout::AddWidgetToLayer(UMWidgetBase* Widget) const
{
	UMWidgetLayer* Layout = GetLayer(WidgetLayout);
	if (Layout)
	{
		Layout->AddWidgetToLayer(Widget);
	}
	else
	{
		MCHAE_WARNING("Layout is null");
	}
}

void UMWidgetLayout::RemoveWidgetFromLayer(UMWidgetBase* Widget) const
{
	UMWidgetLayer* Layout = GetLayer(WidgetLayout);
	if (Layout)
	{
		Layout->RemoveWidgetFromLayer(Widget);
	}
}

bool UMWidgetLayout::RemoveTopWidgetInGameLayer()
{
	UMWidgetLayer* CurrentLayout = GetCurrentLayer();
	if (CurrentLayout)
	{
		return CurrentLayout->RemoveTopWidgetInGameLayer();
	}

	return false;
}

UMWidgetLayer* UMWidgetLayout::GetLayer(EMWidgetLayout InWidgetLayout) const
{
	switch (InWidgetLayout)
	{
		case EMWidgetLayout::GameLayer:
		{
			return GameLayer;
			break;
		}
		case EMWidgetLayout::IndependentLayer:
		{
			return IndependentLayer;
			break;
		}
		default:
		{
			break;
		}
	}

	ensure(false);
	MCHAE_WARNING("Can't found Widget Layout!");
	return nullptr;
}

UMWidgetLayer* UMWidgetLayout::GetCurrentLayer() const
{
	return GetLayer((EMWidgetLayout)LayoutSwitcher->GetActiveWidgetIndex());
}
