// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MWidgetBase.h"
#include "GameplayTagContainer.h"
#include "MContextableItemWidget.generated.h"

class UImage;

UCLASS()
class PROJECTM_API UMContextableItemWidget : public UMWidgetBase
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UMContextableItemWidget(const FObjectInitializer& ObjectInitializer);
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual void NativeDestruct() override;

/*
* Member Functions
*/
public:
	void SetDesiredSizeOverride(FVector2D DesiredSize);
	void SetBrushFromTexture(UTexture2D* Texture, bool bMatchSize = false);
	void SetItemRowId(const int32 InRowId);

protected:
	void EnableContextWidget() const;
	void DisableContextWidget() const;
/*
* Member Variables
*/
protected:
	UPROPERTY(BlueprintReadOnly)
	int32 ItemRowId = INDEX_NONE;
	
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<UImage> ItemImage;
};
