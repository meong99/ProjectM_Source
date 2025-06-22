	// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Inventory/Items/MItemTileWidget.h"
#include "MQuickslotTileWidget.generated.h"

class UImage;
class UTextBlock;

UCLASS(BlueprintType)
class PROJECTM_API UMQuickslotTileWidget : public UMItemTileWidget
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UMQuickslotTileWidget(const FObjectInitializer& ObjectInitializer);
	virtual void	NativeConstruct() override;
	virtual void	NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void	UpdateItemData() override;

	/*
* Member Functions
*/
private:
	UFUNCTION()
	void Callback_OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
	void Callback_OnSetInputComponent(UInputComponent* PlayerInputComponent);
	void OnKeyPressed(const FGameplayTag& KeyTag);

	const FGameplayTag& GetSlotKeyTag() const;
/*
* Member Variables
*/
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> QuickSlotKeyText;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UImage> QuickSlotBackground;
};
