#include "MLoadingWidget.h"
#include "Animation/WidgetAnimation.h"
#include "UI/MViewportClient.h"

UMLoadingWidget::UMLoadingWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

FReply UMLoadingWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	return FReply::Unhandled();
}

FReply UMLoadingWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);

	return FReply::Unhandled();
}

void UMLoadingWidget::PlayFadeIn(FWidgetAnimationDynamicEvent&& Callback)
{
	if (!IsInLayer())
	{
		AddWidgetToLayer(WidgetInfo);
	}

	if (!Callback.IsBound())
	{
		Callback.BindDynamic(this, &UMLoadingWidget::OnFinished_FadeIn);
	}
	BindToAnimationFinished(FadeIn, MoveTemp(Callback));
	PlayAnimation(FadeIn);
}

void UMLoadingWidget::PlayFadeIn(FWidgetAnimationDynamicEvent Callback, bool bAutoRemove)
{
	if (!IsInLayer())
	{
		AddWidgetToLayer(WidgetInfo);
	}

	if (!Callback.IsBound() && bAutoRemove)
	{
		Callback.BindDynamic(this, &UMLoadingWidget::OnFinished_FadeIn);
	}
	BindToAnimationFinished(FadeIn, MoveTemp(Callback));
	PlayAnimation(FadeIn);
}

void UMLoadingWidget::PlayFadeOut(FWidgetAnimationDynamicEvent&& Callback)
{
	if (!IsInLayer())
	{
		AddWidgetToLayer(WidgetInfo);
	}

	if (!Callback.IsBound())
	{
		Callback.BindDynamic(this, &UMLoadingWidget::OnFinished_FadeOut);
	}
	BindToAnimationFinished(FadeOut, MoveTemp(Callback));
	PlayAnimation(FadeOut);
}

void UMLoadingWidget::PlayFadeOut(FWidgetAnimationDynamicEvent Callback, bool bAutoRemove)
{
	if (!IsInLayer())
	{
		AddWidgetToLayer(WidgetInfo);
	}

	if (!Callback.IsBound() && bAutoRemove)
	{
		Callback.BindDynamic(this, &UMLoadingWidget::OnFinished_FadeOut);
	}
	BindToAnimationFinished(FadeOut, MoveTemp(Callback));
	PlayAnimation(FadeOut);
}


void UMLoadingWidget::UnbindAnimationBind()
{
	UnbindAllFromAnimationFinished(FadeIn);
	UnbindAllFromAnimationFinished(FadeOut);
}

void UMLoadingWidget::OnFinished_FadeIn()
{
	RemoveWidgetFromLayer();
}

void UMLoadingWidget::OnFinished_FadeOut()
{
	RemoveWidgetFromLayer();
}
