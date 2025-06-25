// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MWidgetBase.h"
#include "MFloatingWidget.generated.h"

class UTextBlock;
class UImage;
class UTexture2D;

UCLASS()
class PROJECTM_API UMFloatingWidget : public UMWidgetBase
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UMFloatingWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	virtual FReply	NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void	NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void	NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

/*
* Member Functions
*/
public:
protected:
/*
* Member Variables
*/
protected:
	// Floating Target의 이름
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ProjectM")
	FText DisplayText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ProjectM")
	TObjectPtr<UTexture2D> BackgroundImageTextrue;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UTextBlock> FloatingWidgetName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UImage> BackgroundImage;

	bool bIsFloating = false;

	FVector2d DragOffset;
	FVector2d ViewportSize;
	FVector2d WidgetSize;

	UWidget* TargetWidget = nullptr;
};
