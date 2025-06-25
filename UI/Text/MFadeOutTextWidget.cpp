#include "MFadeOutTextWidget.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"

UMFadeOutTextWidget::UMFadeOutTextWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMFadeOutTextWidget::PlayText(const FText& InText)
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	ToastText->SetText(InText);
	bIsActivatedText = true;

	FWidgetAnimationDynamicEvent OnFinishedAnim;
	OnFinishedAnim.BindDynamic(this, &UMFadeOutTextWidget::OnFinished_Anim);

	BindToAnimationFinished(FadeOut, MoveTemp(OnFinishedAnim));
	PlayAnimation(FadeOut);
}

void UMFadeOutTextWidget::DisableSlideWidget()
{
	bIsActivatedText = false;
	if (GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
	UnbindAllFromAnimationFinished(FadeOut);
}

void UMFadeOutTextWidget::OnFinished_Anim()
{
	DisableSlideWidget();
}
