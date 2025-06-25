// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MWidgetBase.h"
#include "MItemWithTextWidget.generated.h"

class UImage;
class UTextBlock;
class UTexture2D;
class UMContextableItemWidget;
class UPMInventoryItemDefinition;

UCLASS()
class PROJECTM_API UMItemWithTextWidget : public UMWidgetBase
{
	GENERATED_BODY()

	/*
	* Overrided Functions
	*/
public:
	UMItemWithTextWidget(const FObjectInitializer& ObjectInitializer);

	/*
	* Member Functions
	*/
public:
	void SetItem(UPMInventoryItemDefinition* ItemDef);
	void SetItemContextText(const FText& ItemContext);
	/*
	* Member Variables
	*/
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UMContextableItemWidget> ItemImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UTextBlock> ItemContextText;
};
