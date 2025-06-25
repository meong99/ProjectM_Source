#pragma once

#include "CoreMinimal.h"
#include "UI/MWidgetBase.h"
#include "NativeGameplayTags.h"
#include "MToastMessageWidget.generated.h"

class UTextBlock;
class UWidgetAnimation;

UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Registry_Game_ToastMessage);

UCLASS()
class PROJECTM_API UMToastMessageWidget : public UMWidgetBase
{
	GENERATED_BODY()

	/*
	* Overrided Functions
	*/
public:
	UMToastMessageWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeOnInitialized() override;

	/*
	* Member Functions
	*/
public:
	static void RequestToastMessage(const UObject* WorldContext, const FText& NewMessage);

	UFUNCTION(BlueprintCallable)
	void AddToastMessage(const FText& NewMessage);

protected:
	void FlushMessageQueue();
	void PlayTopMessage();

	UFUNCTION()
	void OnFinished_ToastAnim();

	/*
	* Member Variables
	*/
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UTextBlock> ToastText;
	
	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim), Category = "ProjectM")
	TObjectPtr<UWidgetAnimation> ToastAnim;

	TQueue<FText> ToastMessageQue;

	bool bIsPlaying = false;
};
