#pragma once

#include "CoreMinimal.h"
#include "UI/MWorldBaseWidget.h"
#include "MInteractionListWidget.generated.h"

class AActor;
class UMInteractionListElementWidget;
class UVerticalBox;
class UMInteractionComponent;

UCLASS()
class PROJECTM_API UMInteractionListWidget : public UMWorldBaseWidget
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UMInteractionListWidget(const FObjectInitializer& ObjectInitializer);
	virtual void	NativeConstruct() override;
	virtual void	NativeDestruct() override;
	virtual FReply	NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
/*
* Member Functions
*/
public:

protected:
	void DeactivateList();

	/*
* Member Variables
*/
protected:
	UPROPERTY(BlueprintReadOnly, Category="ProjectM")
	TObjectPtr<UMInteractionComponent> InteractionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ProjectM")
	TSubclassOf<UMInteractionListElementWidget> ElementWidgetClass;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="ProjectM")
	TObjectPtr<UVerticalBox> InteractionList;
};
