#include "MFloatingWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

UMFloatingWidget::UMFloatingWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMFloatingWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(GetWorld());
	GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
	ViewportSize /= ViewportScale;
	WidgetSize = FVector2d::ZeroVector;

	if (!DisplayText.IsEmpty())
	{
		FloatingWidgetName->SetText(DisplayText);
	}

	if (BackgroundImageTextrue)
	{
		BackgroundImage->SetBrushFromTexture(BackgroundImageTextrue);
	}
}

void UMFloatingWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (bIsFloating && TargetWidget)
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(TargetWidget->Slot);
		if (CanvasSlot)
		{
			FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
			FVector2D NewPosition = MousePosition + DragOffset;

			if (WidgetSize.IsZero())
			{
				WidgetSize = GetDesiredSize();
			}

			if (NewPosition.X < 0)
			{
				NewPosition.X = 0;
			}
			else if (NewPosition.X > ViewportSize.X - WidgetSize.X)
			{
				NewPosition.X = ViewportSize.X - WidgetSize.X;
			}

			if (NewPosition.Y < 0)
			{
				NewPosition.Y = 0;
			}
			else if (NewPosition.Y > ViewportSize.Y - WidgetSize.Y)
			{
				NewPosition.Y = ViewportSize.Y - WidgetSize.Y;
			}

			CanvasSlot->SetPosition(NewPosition);
		}
	}
}

FReply UMFloatingWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UMFloatingWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	if (TargetWidget == nullptr)
	{
		UWidgetTree* OuterWidgetTree = Cast<UWidgetTree>(GetOuter());

		if (OuterWidgetTree)
		{
			TargetWidget = OuterWidgetTree->FindWidget<UWidget>(TEXT("FloatingTarget"));
		}
	}

	if (TargetWidget)
	{
		bIsFloating = true;
		FVector2d MousePosition = InMouseEvent.GetScreenSpacePosition();
		FVector2d WidgetPosition = TargetWidget->GetCachedGeometry().GetAbsolutePosition();

		DragOffset = WidgetPosition - MousePosition;
	}

	OutOperation = UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass());
	if (OutOperation)
	{
		OutOperation->Payload = this;
	}

	return Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
}

void UMFloatingWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bIsFloating = false;
	return Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
}
