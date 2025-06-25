// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MWidgetBase.h"
#include "MNameWidget.generated.h"

class UTextBlock;

UCLASS()
class PROJECTM_API UMNameWidget : public UMWidgetBase
{
	GENERATED_BODY()

	/*
	* Overrided Functions
	*/
public:
	UMNameWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

/*
* Member Functions
*/
public:
	void SetName(const FName& NewName);
	FVector2D GetSize() const;

/*
* Member Variables
*/
protected:
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	UTextBlock* Name;
};
