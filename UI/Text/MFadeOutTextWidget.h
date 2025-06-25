// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MWidgetBase.h"
#include "MFadeOutTextWidget.generated.h"

class UTextBlock;
class UWidgetAnimation;

UCLASS()
class PROJECTM_API UMFadeOutTextWidget : public UMWidgetBase
{
	GENERATED_BODY()

	/*
	* Overrided Functions
	*/
public:
	UMFadeOutTextWidget(const FObjectInitializer& ObjectInitializer);

/*
* Member Functions
*/
public:
	void PlayText(const FText& InText);
	bool IsActivatedText() const { return bIsActivatedText; }

protected:
	void DisableSlideWidget();

	UFUNCTION()
	void OnFinished_Anim();

/*
* Member Variables
*/
protected:
	UPROPERTY(BlueprintReadWrite, Category="ProjectM")
	bool bIsActivatedText = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(BindWidget), Category = "ProjectM")
	TObjectPtr<UTextBlock> ToastText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta=(BindWidgetAnim), Category = "ProjectM")
	TObjectPtr<UWidgetAnimation> FadeOut;
};
