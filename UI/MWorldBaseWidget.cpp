#include "MWorldBaseWidget.h"
#include "GameFramework/Actor.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Util/MGameplayStatics.h"

UMWorldBaseWidget::UMWorldBaseWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMWorldBaseWidget::NativeConstruct()
{
	if (WidgetInfo.WidgetOwnerActor)
	{
		InstigatorActor = Cast<AActor>(WidgetInfo.WidgetOwnerActor);
	}

	CachedController = UGameplayStatics::GetPlayerController(this, 0);
	ViewportSize = UWidgetLayoutLibrary::GetViewportScale(this);

	UpdateBaseWorldLocation();
	Super::NativeConstruct();
}

void UMWorldBaseWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (TargetWidget && CachedController)
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(TargetWidget->Slot);
		if (CanvasSlot)
		{
			FVector2D ScreenPosition;
			CachedController->ProjectWorldLocationToScreen(BaseWorldLocation, ScreenPosition);

			ScreenPosition /= ViewportSize;
			CanvasSlot->SetPosition(ScreenPosition);
		}
	}
}

void UMWorldBaseWidget::UpdateBaseWorldLocation()
{
	if (InstigatorActor)
	{
		BaseWorldLocation = InstigatorActor->GetActorLocation() + AddtiveLocation;
	}
}
