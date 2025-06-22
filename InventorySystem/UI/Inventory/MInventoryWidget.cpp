#include "MInventoryWidget.h"
#include "Input/PMInputComponent.h"
#include "GameFramework/Pawn.h"
#include "Inventory/PMInventoryManagerComponent.h"
#include "Items/MItemTileWidget.h"
#include "Inventory/PMInventoryItemList.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "CommonWidgets/MTileView.h"
#include "MInventoryTemplete.h"
#include "Inventory/PMInventoryItemInstance.h"
#include "Player/PMPlayerControllerBase.h"
#include "AbilitySystem/PMAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/PMCombatSet.h"
#include "AbilitySystem/Attributes/PMHealthSet.h"
#include "Components/TextBlock.h"

UMInventoryWidget::UMInventoryWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BindDelegates();
}

void UMInventoryWidget::Callback_OnInitInventory(const FPMInventoryItemList& InventoryList)
{
	UMInventoryTemplete* InvenOfType = Cast<UMInventoryTemplete>(CreateWidget(this, InventoryTemplete));
	if (InvenOfType)
	{
		Inventories.Add(InvenOfType->GetTileView());
		InventoryTileSwitcher->AddChild(InvenOfType);
	}
	InitInventorySlots(InventoryList);
}

void UMInventoryWidget::Callback_AddNewItem(const FMItemResponse& ItemRespons)
{
	if (ItemRespons.ResponsType == EMItemResponseType::TotallyNewItem)
	{
		APlayerController* PlayerController = GetOwningPlayer();
		InventoryComponent = PlayerController ? PlayerController->FindComponentByClass<UPMInventoryManagerComponent>() : nullptr;
		if (IsValid(InventoryComponent) && ItemRespons.ItemRequest.ItemInstance)
		{
			FPMInventoryEntry* Entry = InventoryComponent->FindEntry(ItemRespons.ItemRequest.ItemInstance->ItemHandle);
			if (Entry)
			{
				UMTileView* View = GetItemSlotView(Entry->GetItemType());
				if (View)
				{
					View->AddNewItem(*Entry);
				}
			}
		}
	}
}

void UMInventoryWidget::Callback_RemoveItem(const FMItemResponse& ItemRespons)
{
	if (ItemRespons.ResponsType == EMItemResponseType::Removed && ItemRespons.ItemRequest.ItemInstance)
	{
		UMTileView* View = GetItemSlotView(ItemRespons.ItemRequest.ItemInstance->GetItemType());
		if (View)
		{
			View->RemoveItem(ItemRespons.ItemRequest.ItemHandle);
		}
	}
}

void UMInventoryWidget::BindDelegates()
{
	APMPlayerControllerBase* PlayerController = Cast<APMPlayerControllerBase>(GetOwningPlayer());
	InventoryComponent = PlayerController ? PlayerController->FindComponentByClass<UPMInventoryManagerComponent>() : nullptr;
	if (IsValid(InventoryComponent))
	{
		InventoryComponent->CallOrRegister_OnInitInventory(FOnInitInventory::FDelegate::CreateUObject(this, &UMInventoryWidget::Callback_OnInitInventory));
		InventoryComponent->Delegate_OnItemIncreased.AddUObject(this, &UMInventoryWidget::Callback_AddNewItem);
		InventoryComponent->Delegate_OnRemoveItem.AddUObject(this, &UMInventoryWidget::Callback_RemoveItem);
	}
	else
	{
		ensure(false);
		MCHAE_ERROR("Can't get inventorycomponent!");
	}

	UPMAbilitySystemComponent* ASC = PlayerController ? PlayerController->GetAbilitySystemComponent() : nullptr;
	if (ASC)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(UPMCombatSet::GetAttackPowerAttribute()).AddUObject(this, &UMInventoryWidget::Callback_CombatChange);
		ASC->GetGameplayAttributeValueChangeDelegate(UPMCombatSet::GetDefensePowerAttribute()).AddUObject(this, &UMInventoryWidget::Callback_CombatChange);
		ASC->GetGameplayAttributeValueChangeDelegate(UPMHealthSet::GetMaxHealthAttribute()).AddUObject(this, &UMInventoryWidget::Callback_HealthChange);
		ASC->GetGameplayAttributeValueChangeDelegate(UPMHealthSet::GetHealthRecoveryAttribute()).AddUObject(this, &UMInventoryWidget::Callback_HealthChange);
		const UPMCombatSet* CombatSet = ASC->GetSet<UPMCombatSet>();
		if (CombatSet)
		{
			SetCombatStat(CombatSet->GetAttackPowerAttribute(), 0, CombatSet->GetAttackPower(), nullptr);
			SetCombatStat(CombatSet->GetDefensePowerAttribute(), 0, CombatSet->GetDefensePower(), nullptr);
		}
		const UPMHealthSet* HealthSet = ASC->GetSet<UPMHealthSet>();
		if (HealthSet)
		{
			OnChange_MaxHealth(0, HealthSet->GetMaxHealth(), nullptr);
			OnChange_HealthRecovery(0, HealthSet->GetHealthRecovery(), nullptr);
		}
	}

	if (EquipmentButton_Deactivated)
	{
		EquipmentButton_Deactivated->OnClicked.AddDynamic(this, &UMInventoryWidget::OnClick_EquipmentButton);
	}
	if (ConsumableButton_Deactivated)
	{
		ConsumableButton_Deactivated->OnClicked.AddDynamic(this, &UMInventoryWidget::OnClick_ConsumableButton);
	}
	if (MiscellaneousButton_Deactivated)
	{
		MiscellaneousButton_Deactivated->OnClicked.AddDynamic(this, &UMInventoryWidget::OnClick_MiscellaneousButton);
	}
	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UMInventoryWidget::OnClick_ExitButton);
	}
}

void UMInventoryWidget::InitInventorySlots(const FPMInventoryItemList& InventoryList)
{
	InitInventorySlots_Impl(InventoryList, GetItemSlotView(InventoryList.OwnedItemType));
}

void UMInventoryWidget::InitInventorySlots_Impl(const FPMInventoryItemList& InventoryList, UMTileView* ItemSlotView)
{
	if (InventoryComponent && ItemSlotView)
	{
		const int32 MaxInventoryCount = InventoryComponent->GetMaxInventoryCount();
		ItemSlotView->InitView(InventoryList, MaxInventoryCount);
	}
}

UMTileView* UMInventoryWidget::GetItemSlotView(const EMItemType ItemType)
{
	if (Inventories.IsValidIndex((int32)ItemType))
	{
		return Inventories[(int32)ItemType];
	}

	return nullptr;
}

void UMInventoryWidget::OnClick_EquipmentButton()
{
	InventoryTileSwitcher->SetActiveWidgetIndex((int32)EMItemType::Equipment);
	EquipButtonSwitcher->SetActiveWidgetIndex((int32)EMButtonState::Active);
	ConsumableButtonSwitcher->SetActiveWidgetIndex((int32)EMButtonState::Deactive);
	MiscellaneousButtonSwitcher->SetActiveWidgetIndex((int32)EMButtonState::Deactive);
}

void UMInventoryWidget::OnClick_ConsumableButton()
{
	InventoryTileSwitcher->SetActiveWidgetIndex((int32)EMItemType::Consumable);
	EquipButtonSwitcher->SetActiveWidgetIndex((int32)EMButtonState::Deactive);
	ConsumableButtonSwitcher->SetActiveWidgetIndex((int32)EMButtonState::Active);
	MiscellaneousButtonSwitcher->SetActiveWidgetIndex((int32)EMButtonState::Deactive);
}

void UMInventoryWidget::OnClick_MiscellaneousButton()
{
	InventoryTileSwitcher->SetActiveWidgetIndex((int32)EMItemType::Miscellaneous);
	EquipButtonSwitcher->SetActiveWidgetIndex((int32)EMButtonState::Deactive);
	ConsumableButtonSwitcher->SetActiveWidgetIndex((int32)EMButtonState::Deactive);
	MiscellaneousButtonSwitcher->SetActiveWidgetIndex((int32)EMButtonState::Active);
}

void UMInventoryWidget::OnClick_ExitButton()
{
	RemoveWidgetFromLayer();
}

void UMInventoryWidget::Callback_CombatChange(const FOnAttributeChangeData& ChangeData)
{
	float OldValue = ChangeData.OldValue;
	float NewValue = ChangeData.NewValue;
	AActor* Instigator = UPMAbilitySystemComponent::GetInstigatorFromAttrChangeData(ChangeData);

	SetCombatStat(ChangeData.Attribute, OldValue, NewValue, Instigator);
}

void UMInventoryWidget::Callback_HealthChange(const FOnAttributeChangeData& ChangeData)
{
	float OldValue = ChangeData.OldValue;
	float NewValue = ChangeData.NewValue;
	AActor* Instigator = UPMAbilitySystemComponent::GetInstigatorFromAttrChangeData(ChangeData);

	if (ChangeData.Attribute == UPMHealthSet::GetMaxHealthAttribute())
	{
		OnChange_MaxHealth(OldValue, NewValue, Instigator);
	}
	else if (ChangeData.Attribute == UPMHealthSet::GetHealthRecoveryAttribute())
	{
		OnChange_HealthRecovery(OldValue, NewValue, Instigator);
	}
}

void UMInventoryWidget::SetCombatStat(const FGameplayAttribute& Attribute, const float OldValue, const float NewValue, AActor* Instigator)
{
	if (UPMCombatSet::GetAttackPowerAttribute() == Attribute)
	{
		OnChange_AttackPower(OldValue, NewValue, Instigator);
	}
	else if (UPMCombatSet::GetDefensePowerAttribute() == Attribute)
	{
		OnChange_DefencePower(OldValue, NewValue, Instigator);
	}
}

void UMInventoryWidget::OnChange_MaxHealth(const float OldValue, const float NewValue, AActor* Instigator)
{
	const FText& NewText = MakeFormatText(TEXT("최대 체력"), NewValue);
	MaxHealth->SetText(NewText);
}

void UMInventoryWidget::OnChange_HealthRecovery(const float OldValue, const float NewValue, AActor* Instigator)
{
	const FText& NewText = MakeFormatText(TEXT("초당 체력 회복"), NewValue);
	HealthRecovery->SetText(NewText);
}

void UMInventoryWidget::OnChange_AttackPower(const float OldValue, const float NewValue, AActor* Instigator)
{
	const FText& NewText = MakeFormatText(TEXT("공격력"), NewValue);
	AttackPower->SetText(NewText);
}

void UMInventoryWidget::OnChange_DefencePower(const float OldValue, const float NewValue, AActor* Instigator)
{
	const FText& NewText = MakeFormatText(TEXT("방어력"), NewValue);
	DefencePower->SetText(NewText);
}

FText UMInventoryWidget::MakeFormatText(const FString& Name, const float Value)
{
	FText Template = FText::FromString("{Name} : {Value}");
	FFormatNamedArguments Args;

	Args.Add("Name", FText::FromString(Name));
	Args.Add("Value", FText::AsNumber(Value));

	return FText::Format(Template, Args);
}
