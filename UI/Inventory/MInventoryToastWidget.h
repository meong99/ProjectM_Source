// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MWidgetBase.h"
#include "Inventory/MInventoryTypes.h"
#include "MInventoryToastWidget.generated.h"

class UPMInventoryManagerComponent;
class UMWalletComponent;
class UPMExperienceDefinition;
class UMFadeOutTextWidget;
class UVerticalBox;

UCLASS()
class PROJECTM_API UMInventoryToastWidget : public UMWidgetBase
{
	GENERATED_BODY()

	/*
	* Overrided Functions
	*/
public:
	UMInventoryToastWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeOnInitialized() override;

/*
* Member Functions
*/
public:
	UFUNCTION(BlueprintCallable)
	void InitThisWidget();

	UFUNCTION()
	void OnChangeGold(int64 AdjustPrice, int64 NewGold);

	void OnNotifyNewItem(const FMItemResponse& ItemRespons);

protected:
	void OnExperienceLoaded(const UPMExperienceDefinition* LoadedExperienceDefinition);
	
	UMFadeOutTextWidget* GetLastUsedWidget();

	void					PushQue(UMFadeOutTextWidget* Widget);
	UMFadeOutTextWidget*	PopQue();

/*
* Member Variables
*/
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(BindWidget), Category="ProjectM")
	TObjectPtr<UVerticalBox> TextVerticalBox;

	UPROPERTY(BlueprintReadOnly)
	UPMInventoryManagerComponent* InvenManager;

	UPROPERTY(BlueprintReadOnly)
	UMWalletComponent* WalletComponent;

	TQueue<UMFadeOutTextWidget*> ActivateWidgetQue;

	int32 QueSize = 0;
};
