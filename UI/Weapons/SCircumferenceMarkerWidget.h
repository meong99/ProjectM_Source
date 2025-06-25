#pragma once

#include "Containers/Array.h"
#include "Math/Vector2D.h"
#include "Misc/Attribute.h"
#include "Rendering/SlateRenderTransform.h"
#include "Styling/CoreStyle.h"
#include "Styling/ISlateStyle.h"
#include "Styling/SlateColor.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SLeafWidget.h"
#include "SCircumferenceMarkerWidget.generated.h"

USTRUCT(BlueprintType)
struct FCircumferenceMarkerEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ForceUnits = deg))
	float PositionAngle = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (FOrceUnits = deg))
	float ImageRotationAngle = 0.0f;
};

class SCircumferenceMarkerWidget : public SLeafWidget
{
	SLATE_BEGIN_ARGS(SCircumferenceMarkerWidget)
		: _MarkerBrush(FCoreStyle::Get().GetBrush("Throbber.CircleChunk"))
		, _Radius(48.0f)
		{}
		SLATE_ARGUMENT(const FSlateBrush*, MarkerBrush)
		SLATE_ARGUMENT(TArray<FCircumferenceMarkerEntry>, MarkerList)
		SLATE_ATTRIBUTE(float, Radius)
		SLATE_ATTRIBUTE(FSlateColor, ColorAndOpacity)
	SLATE_END_ARGS()

public:
	SCircumferenceMarkerWidget();

	void Construct(const FArguments& InArgs);

	void SetRadius(float NewRadius);
	void SetMarkerList(TArray<FCircumferenceMarkerEntry>& NewMarkerList);
	FSlateRenderTransform GetMarkerRenderTransform(const FCircumferenceMarkerEntry& Marker, const float BaseRadius, const float HUDScale) const;

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FVector2D ComputeDesiredSize(float) const override;
	virtual bool ComputeVolatility() const override { return true; }

	const FSlateBrush* MarkerBrush;

	TArray<FCircumferenceMarkerEntry> MarkerList;

	TAttribute<float> Radius;

	TAttribute<FSlateColor> ColorAndOpacity;
	bool bColorAndOpacitySet;

	uint8 bRecticleCornerOutsideSpreadRadius : 1;
};