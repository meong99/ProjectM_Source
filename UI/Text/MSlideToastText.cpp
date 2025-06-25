#include "MSlideToastText.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"

UMSlideToastText::UMSlideToastText(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMSlideToastText::PlayDamageText(const FText& InText)
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	ToastText->SetText(InText);
	bIsActivatedText = true;

	FWidgetAnimationDynamicEvent OnFinishedAnim;
	OnFinishedAnim.BindDynamic(this, &UMSlideToastText::OnFinished_Anim);

	BindToAnimationFinished(SlideWidgetAnim, MoveTemp(OnFinishedAnim));
	PlayAnimation(SlideWidgetAnim);
}

void UMSlideToastText::DisableSlideWidget()
{
	bIsActivatedText = false;
	if (GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
	UnbindAllFromAnimationFinished(SlideWidgetAnim);
}

void UMSlideToastText::OnFinished_Anim()
{
	DisableSlideWidget();
}
