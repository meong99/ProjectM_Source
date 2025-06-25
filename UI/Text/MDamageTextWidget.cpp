#include "MDamageTextWidget.h"
#include "MSlideToastText.h"
#include "Components/Overlay.h"

UMDamageTextWidget::UMDamageTextWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMDamageTextWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	TArray<UWidget*> Children = TextOverlay->GetAllChildren();
	for (int32 i = 0; i < Children.Num(); i++)
	{
		UMSlideToastText* Widget = Cast<UMSlideToastText>(Children[i]);
		if (Widget)
		{
			PushQue(Widget);
		}
	}
}

void UMDamageTextWidget::OnDamaged(const float Damage)
{
	UMSlideToastText* Widget = GetLastUsedWidget();
	if (Widget)
	{
		// 데미지 표기만 버림 후 표시.
		int32 DamageInt = FMath::FloorToInt32(Damage);
		Widget->PlayDamageText(FText::AsNumber(DamageInt));
	}
}

UMSlideToastText* UMDamageTextWidget::GetLastUsedWidget()
{
	UMSlideToastText* Widget = PopQue();
	PushQue(Widget);
	return Widget;
}

void UMDamageTextWidget::PushQue(UMSlideToastText* Widget)
{
	if (Widget)
	{
		ActivateWidgetQue.Enqueue(Widget);
		++QueSize;
	}
}

UMSlideToastText* UMDamageTextWidget::PopQue()
{
	UMSlideToastText* Widget = nullptr;
	if (!ActivateWidgetQue.IsEmpty())
	{
		ActivateWidgetQue.Peek(Widget);
		ActivateWidgetQue.Pop();
		--QueSize;
	}
	
	return Widget;
}
