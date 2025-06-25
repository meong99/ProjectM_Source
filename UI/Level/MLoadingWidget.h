// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MWidgetBase.h"
#include "MLoadingWidget.generated.h"

class UWidgetAnimation;

UCLASS()
class PROJECTM_API UMLoadingWidget : public UMWidgetBase
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UMLoadingWidget(const FObjectInitializer& ObjectInitializer);
	virtual FReply	NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply	NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

/*
* Member Functions
*/
public:
	// Bind하지 않고 호출 시 자동으로 Remove됨
	void PlayFadeIn(FWidgetAnimationDynamicEvent&& Callback);
	UFUNCTION(BlueprintCallable)
	void PlayFadeIn(FWidgetAnimationDynamicEvent Callback, bool bAutoRemove);

	// Bind하지 않고 호출 시 자동으로 Remove됨
	void PlayFadeOut(FWidgetAnimationDynamicEvent&& Callback);
	UFUNCTION(BlueprintCallable)
	void PlayFadeOut(FWidgetAnimationDynamicEvent Callback, bool bAutoRemove);

	void UnbindAnimationBind();
protected:

	UFUNCTION()
	void OnFinished_FadeIn();

	UFUNCTION()
	void OnFinished_FadeOut();
/*
* Member Variables
*/
protected:
	UPROPERTY(EditDefaultsOnly, Transient, meta = (BindWidgetAnim), Category = "ProjectM")
	TObjectPtr<UWidgetAnimation> FadeIn;

	UPROPERTY(EditDefaultsOnly, Transient, meta = (BindWidgetAnim), Category = "ProjectM")
	TObjectPtr<UWidgetAnimation> FadeOut;
};
