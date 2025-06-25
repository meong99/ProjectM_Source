// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MWidgetBase.h"
#include "MDefaultShopWidget.generated.h"

class UButton;
class UMDefaultShopDetailWidget;
class UTextBlock;

UCLASS(Abstract)
class PROJECTM_API UMDefaultShopWidget : public UMWidgetBase
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UMDefaultShopWidget(const FObjectInitializer& ObjectInitializer);
protected:
	virtual void PreAddToLayer() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

/*
* Member Functions
*/
protected:
	void DeactivateShop();

	UFUNCTION()
	void OnClicked_ExitButton();

	UFUNCTION()
	void OnChange_Gold(int64 AdjustNum, int64 NewGold);
/*
* Member Variables
*/
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UButton> ExitButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UTextBlock> ShopName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UTextBlock> OwnedGold;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UMDefaultShopDetailWidget> ShopDetail;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UMDefaultShopDetailWidget> UserInventoryDetail;
};
