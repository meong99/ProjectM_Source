// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MWidgetBase.h"
#include "MWidgetLayout.generated.h"

class UOverlay;
class UWidgetSwitcher;
class UMWidgetLayer;

UCLASS()
class PROJECTM_API UMWidgetLayout : public UMWidgetBase
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UMWidgetLayout(const FObjectInitializer& ObjectInitializer);
	virtual void PreAddToLayer() override;
	virtual void NativeConstruct() override;
	virtual FReply	NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply	NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/*
* Member Functions
*/
public:
	void MakeLayerById(UMWidgetLayer* MainLayer);
	void ChangeWidgetLayer(EMWidgetLayout NewWidgetLayout) const;
	void ClearAllWidget();

	void AddWidgetToCurrentLayer(UMWidgetBase* Widget) const;
	void RemoveWidgetToCurrentLayer(UMWidgetBase* Widget) const;
	void AddWidgetToLayer(UMWidgetBase* Widget) const;
	void RemoveWidgetFromLayer(UMWidgetBase* Widget) const;

	bool RemoveTopWidgetInGameLayer();
private:
	UMWidgetLayer* GetLayer(EMWidgetLayout InWidgetLayout) const;
	UMWidgetLayer* GetCurrentLayer() const;
/*
* Member Variables
*/
protected:
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* LayoutSwitcher;

	UPROPERTY(meta = (BindWidget))
	UMWidgetLayer* GameLayer;

	UPROPERTY(meta = (BindWidget))
	UMWidgetLayer* IndependentLayer;
};
