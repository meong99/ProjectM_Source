// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MWidgetBase.h"
#include "Util/MPriorityQueue.h"
#include "Inventory/PMInventoryItemList.h"
#include "GameplayEffectTypes.h"
#include "MInventoryWidget.generated.h"

class UTileView;
class UPMInventoryManagerComponent;
class UMItemTileWidget;

struct FPMInventoryEntry;
class UButton;
class UWidgetSwitcher;
class UMTileView;
class UMInventoryTemplete;
class UTextBlock;

UCLASS()
class PROJECTM_API UMInventoryWidget : public UMWidgetBase
{
	GENERATED_BODY()

	enum EMButtonState
	{
		Active,
		Deactive,
	};

/*
* Overrided Functions
*/
public:
	UMInventoryWidget(const FObjectInitializer& ObjectInitializer);

	virtual void	NativeOnInitialized() override;

/*
* Member Functions
*/
public:
	void	Callback_OnInitInventory(const FPMInventoryItemList& InventoryList);

	//인벤토리에 존재하지 않던 새로운 아이템이 추가됨
	void	Callback_AddNewItem(const FMItemResponse& ItemRespons);
	void	Callback_RemoveItem(const FMItemResponse& ItemRespons);

protected:
	void	BindDelegates();
	void	InitInventorySlots(const FPMInventoryItemList& InventoryList);
	void	InitInventorySlots_Impl(const FPMInventoryItemList& InventoryList, UMTileView* ItemSlotView);

	UMTileView* GetItemSlotView(const EMItemType ItemType);

	UFUNCTION()
	void	OnClick_EquipmentButton();
	UFUNCTION()
	void	OnClick_ConsumableButton();
	UFUNCTION()
	void	OnClick_MiscellaneousButton();
	UFUNCTION()
	void	OnClick_ExitButton();

	void	Callback_CombatChange(const FOnAttributeChangeData& ChangeData);
	void	Callback_HealthChange(const FOnAttributeChangeData& ChangeData);
	void	SetCombatStat(const FGameplayAttribute& Attribute, const float OldValue, const float NewValue, AActor* Instigator);
	void	OnChange_MaxHealth(const float OldValue, const float NewValue, AActor* Instigator);
	void	OnChange_HealthRecovery(const float OldValue, const float NewValue, AActor* Instigator);
	void	OnChange_AttackPower(const float OldValue, const float NewValue, AActor* Instigator);
	void	OnChange_DefencePower(const float OldValue, const float NewValue, AActor* Instigator);
	FText	MakeFormatText(const FString& Name, const float Value);

/*
* Member Variables
*/
protected:
	UPROPERTY()
	UPMInventoryManagerComponent* InventoryComponent;

	UPROPERTY(EditDefaultsOnly, Category="ProjectM")
	TSubclassOf<UMInventoryTemplete> InventoryTemplete;

	// 타입 별 인벤토리
	TArray<UMTileView*> Inventories;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> InventoryTileSwitcher;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> EquipButtonSwitcher;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> ConsumableButtonSwitcher;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> MiscellaneousButtonSwitcher;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> EquipmentButton_Deactivated;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ConsumableButton_Deactivated;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MiscellaneousButton_Deactivated;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ExitButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MaxHealth;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HealthRecovery;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AttackPower;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DefencePower;
};
