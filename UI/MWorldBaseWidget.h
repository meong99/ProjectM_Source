// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MWidgetBase.h"
#include "MWorldBaseWidget.generated.h"

class UCanvasPanel;
class AActor;
class UPanelWidget;
class APlayerController;

UCLASS()
class PROJECTM_API UMWorldBaseWidget : public UMWidgetBase
{
	GENERATED_BODY()

	/*
	* Overrided Functions
	*/
public:
	UMWorldBaseWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

/*
* Member Functions
*/
public:
	void UpdateBaseWorldLocation();
/*
* Member Variables
*/
protected:
	UPROPERTY(BlueprintReadWrite)
	FVector BaseWorldLocation;

	UPROPERTY(EditDefaultsOnly)
	FVector AddtiveLocation;

	UPROPERTY(BlueprintReadOnly)
	AActor* InstigatorActor;

	UPROPERTY(BlueprintReadOnly)
	APlayerController* CachedController;

	UPROPERTY(BlueprintReadOnly)
	float ViewportSize = 1;

	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* CanvasPanel;

	UPROPERTY(meta=(BindWidget))
	UPanelWidget* TargetWidget;
};
