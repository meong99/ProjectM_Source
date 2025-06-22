#pragma once

#include "CoreMinimal.h"
#include "UI/MWidgetBase.h"
#include "MInteractionListElementWidget.generated.h"

class AActor;
class UTextBlock;
class UImage;
class UTexture2D;
class UMInteractiveAction_OnInteractionBase;

UCLASS()
class PROJECTM_API UMInteractionListElementWidget : public UMWidgetBase
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UMInteractionListElementWidget(const FObjectInitializer& ObjectInitializer);
	FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

/*
* Member Functions
*/
public:
	void SetInteractiveAction(UMInteractiveAction_OnInteractionBase* InInteractiveAction);

protected:
	void SetActionName(const FText& InActionName);
	void SetBackgroundImage(UTexture2D* InBackgroundImage);

/*
* Member Variables
*/
protected:
	UPROPERTY(BlueprintReadOnly, Category="ProjectM")
	TObjectPtr<UMInteractiveAction_OnInteractionBase> InteractiveAction;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="ProjectM")
	TObjectPtr<UTextBlock> ActionName;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="ProjectM")
	TObjectPtr<UImage> BackgroundImage;
};
