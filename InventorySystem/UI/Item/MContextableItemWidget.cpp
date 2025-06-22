#include "MContextableItemWidget.h"
#include "Components/Image.h"
#include "UI/MViewportClient.h"
#include "PMGameplayTags.h"
#include "MItemContextWidget.h"

UMContextableItemWidget::UMContextableItemWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

FReply UMContextableItemWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
	return FReply::Unhandled();
}

void UMContextableItemWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	EnableContextWidget();
}

void UMContextableItemWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	DisableContextWidget();
}

void UMContextableItemWidget::NativeDestruct()
{
	Super::NativeDestruct();
	DisableContextWidget();
}

void UMContextableItemWidget::SetDesiredSizeOverride(FVector2D DesiredSize)
{
	ItemImage->SetDesiredSizeOverride(DesiredSize);
}

void UMContextableItemWidget::SetBrushFromTexture(UTexture2D* Texture, bool bMatchSize /*= false*/)
{
	ItemImage->SetBrushFromTexture(Texture, bMatchSize);
}

void UMContextableItemWidget::SetItemRowId(const int32 InRowId)
{
	ItemRowId = InRowId;
}

void UMContextableItemWidget::EnableContextWidget() const
{
	UMViewportClient* ViewportClient = UMViewportClient::Get(this);
	if (ViewportClient)
	{
		UMItemContextWidget* ContextWidget = Cast<UMItemContextWidget>(ViewportClient->AddWidgetToLayer(FPMGameplayTags::Get().UI_Registry_Game_ItemContext));
		if (ContextWidget)
		{
			ContextWidget->UpdateContextWidget(ItemRowId);
		}
	}
}

void UMContextableItemWidget::DisableContextWidget() const
{
	UMViewportClient* ViewportClient = UMViewportClient::Get(this);
	if (ViewportClient)
	{
		ViewportClient->RemoveWidgetFromLayer(FPMGameplayTags::Get().UI_Registry_Game_ItemContext);
	}
}
