#pragma once

#include "CoreMinimal.h"
#include "UI/MWidgetBase.h"
#include "MDialogueBoxWidget.generated.h"

class UTextBlock;
class UButton;
class UHorizontalBox;

UCLASS()
class PROJECTM_API UMDialogueBoxWidget : public UMWidgetBase
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UMDialogueBoxWidget(const FObjectInitializer& ObjectInitializer);
	virtual void	NativeOnInitialized() override;
	virtual void	NativeConstruct() override;
	virtual void	NativeDestruct() override;
	virtual FReply	NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

/*
* Member Functions
*/
public:
	void InitDialogueWidget(const int32 DialogueRowId);
	void SetCallback1(TFunction<void ()> Callback);
	void SetCallback2(TFunction<void ()> Callback);

protected:
	void StartDialogue();
	void SetDialogue();
	void TurnOverPage();
	void ClearDialogue();
	
	UFUNCTION()
	void OnClick_Button1();
	UFUNCTION()
	void OnClick_Button2();
	UFUNCTION()
	void OnClick_ExitButton();

	void SetOwnerName(const FText InOwnerName);
	void SetButton1_Text(const FText& InText);
	void SetButton2_Text(const FText& InText);

/*
* Member Variables
*/
protected:
	TFunction<void()> Callback1;
	TFunction<void()> Callback2;

	FText Button1_Text;
	FText Button2_Text;

	int32 CurrentPage = 0;
	
	UPROPERTY()
	TArray<FText> DialogTextArray;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UHorizontalBox> ButtonBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UTextBlock> OwnerName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UTextBlock> MainDialog;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UTextBlock> Button1Text;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UTextBlock> Button2Text;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UButton> CallbackButton1;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UButton> CallbackButton2;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UButton> ExitButton;
};
