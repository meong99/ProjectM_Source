#include "MToastMessageWidget.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"
#include "UI/MViewportClient.h"

UE_DEFINE_GAMEPLAY_TAG(UI_Registry_Game_ToastMessage, "UI.Registry.Game.ToastMessage");

UMToastMessageWidget::UMToastMessageWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMToastMessageWidget::NativeOnInitialized()
{
	FWidgetAnimationDynamicEvent FinishEvent;
	FinishEvent.BindDynamic(this, &UMToastMessageWidget::OnFinished_ToastAnim);
	BindToAnimationFinished(ToastAnim, FinishEvent);
}

void UMToastMessageWidget::RequestToastMessage(const UObject* WorldContext, const FText& NewMessage)
{
	UMViewportClient* ViewportClient = UMViewportClient::Get(WorldContext);
	if (ViewportClient)
	{
		UMToastMessageWidget* ToastWidget = Cast<UMToastMessageWidget>(ViewportClient->AddWidgetToLayer(UI_Registry_Game_ToastMessage));
		if (ToastWidget)
		{
			ToastWidget->AddToastMessage(NewMessage);
		}
	}
}

void UMToastMessageWidget::AddToastMessage(const FText& NewMessage)
{
	ToastMessageQue.Enqueue(NewMessage);
	FlushMessageQueue();
}

void UMToastMessageWidget::FlushMessageQueue()
{
	if (!bIsPlaying)
	{
		PlayTopMessage();
	}
}

void UMToastMessageWidget::PlayTopMessage()
{
	if (!ToastMessageQue.IsEmpty())
	{
		FText NewMessage;
		ToastMessageQue.Dequeue(NewMessage);
		ToastText->SetText(NewMessage);

		PlayAnimation(ToastAnim);
		bIsPlaying = true;
	}
	else
	{
		bIsPlaying = false;
	}
}

void UMToastMessageWidget::OnFinished_ToastAnim()
{
	bIsPlaying = false;
	FlushMessageQueue();
}
