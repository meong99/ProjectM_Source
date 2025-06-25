// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MWidgetBase.h"
#include "MItemContextWidget.generated.h"

class UImage;
class UOverlay;
class UTextBlock;

UCLASS()
class PROJECTM_API UMItemContextWidget : public UMWidgetBase
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UMItemContextWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;

/*
* Member Functions
*/
public:
	void UpdateContextWidget(const int32 RowId);

protected:
	void SetWidgetPosition();
	void DisableTexts();
	void SetItemImage(UTexture2D* NewTexture);
	void SetItemContextText(const FText& NewContext);
	void SetItemName(const FText& NewName);
	void SetSpecText(UTextBlock* InTextBlock, const FText& InName, const float Value);
	void GetTextBlockByTag(const FGameplayTag& AttributeTag, UTextBlock*& OutTextBlock, FText& OutName);

/*
* Member Variables
*/
protected:
	UPROPERTY(BlueprintReadOnly)
	int32 ItemRowId = INDEX_NONE;

	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<UImage> ItemImage;

	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<UOverlay> FloatingTarget;

	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<UOverlay> StatOverlay;

	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> ItemName;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemContext;

	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> Health;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> MaxHealth;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> AttackPower;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> DefencePower;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> HealthRecovery;
};
