#include "MQuickslotTileWidget.h"
#include "Components/TextBlock.h"
#include "Input/Events.h"
#include "Inventory/PMInventoryManagerComponent.h"
#include "GameFramework/Pawn.h"
#include "Input/PMInputComponent.h"
#include "PMGameplayTags.h"
#include "Character/MPlayerCharacterBase.h"
#include "Components/Image.h"

UMQuickslotTileWidget::UMQuickslotTileWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMQuickslotTileWidget::NativeConstruct()
{
	Super::NativeConstruct();

	APlayerController* Controller = GetOwningPlayer();
	if (Controller)
	{
		Controller->OnPossessedPawnChanged.AddDynamic(this, &UMQuickslotTileWidget::Callback_OnPossessedPawnChanged);
		if (Controller->GetPawn())
		{
			Callback_OnPossessedPawnChanged(nullptr, Controller->GetPawn());
		}
	}
}

void UMQuickslotTileWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	Super::NativeOnListItemObjectSet(ListItemObject);

	UMItemDetailData* ItemData = Cast<UMItemDetailData>(ListItemObject);
	if (ItemData == nullptr)
	{
		return;
	}

	if (ItemData->SlotIndex != INDEX_NONE)
	{
		QuickSlotKeyText->SetText(FText::AsNumber(ItemData->SlotIndex + 1));
	}

	APlayerController*		Controller = GetOwningPlayer();
	AMPlayerCharacterBase*	Character = Controller ? Cast<AMPlayerCharacterBase>(Controller->GetPawn()) : nullptr;
	if (Character)
	{
		Character->CallOrRegister_OnSetInputComponent(FOnSetInputComponent::FDelegate::CreateUObject(this, &UMQuickslotTileWidget::Callback_OnSetInputComponent));
	}
	else
	{
		ensure(false);
		MCHAE_ERROR("Character is not valid!!! So Can't bind quickslot key!");
	}
}

void UMQuickslotTileWidget::UpdateItemData()
{
	Super::UpdateItemData();

	if (ItemHandle.IsValid())
	{
		QuickSlotBackground->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		QuickSlotBackground->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UMQuickslotTileWidget::Callback_OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	AMPlayerCharacterBase* Character = Cast<AMPlayerCharacterBase>(NewPawn);
	if (Character)
	{
		Character->CallOrRegister_OnSetInputComponent(FOnSetInputComponent::FDelegate::CreateUObject(this, &UMQuickslotTileWidget::Callback_OnSetInputComponent));
	}
}

void UMQuickslotTileWidget::Callback_OnSetInputComponent(UInputComponent* PlayerInputComponent)
{
	UPMInputComponent* PawnInputComponent = Cast<UPMInputComponent>(PlayerInputComponent);
	const FGameplayTag& KeyTag = GetSlotKeyTag();
	if (KeyTag.IsValid())
	{
		PawnInputComponent->Delegate_InputActionMap.Add(KeyTag, FInputActionDelegate::FDelegate::CreateUObject(this, &UMQuickslotTileWidget::OnKeyPressed));
	}
}

void UMQuickslotTileWidget::OnKeyPressed(const FGameplayTag& KeyTag)
{
	if (ItemHandle.IsValid())
	{
		APlayerController* Controller = GetOwningPlayer();
		UPMInventoryManagerComponent* InventoryManagerComp = Controller ? Controller->FindComponentByClass<UPMInventoryManagerComponent>() : nullptr;
		if (InventoryManagerComp)
		{
			InventoryManagerComp->Server_UseItem(ItemHandle);
		}
		else
		{
			MCHAE_WARNING("Can't found InventoryComponent!");
		}
	}
}

const FGameplayTag& UMQuickslotTileWidget::GetSlotKeyTag() const
{
	UMItemDetailData* ItemData = GetItemDatailData();
	if (ItemData == nullptr)
	{
		return FGameplayTag::EmptyTag;
	}

	if (ItemData->SlotIndex == INDEX_NONE)
	{
		return FGameplayTag::EmptyTag;
	}

	switch (ItemData->SlotIndex)
	{
		case 0 :
		{
			return FPMGameplayTags::Get().InputTag_Key_1;
		}
		case 1:
		{
			return FPMGameplayTags::Get().InputTag_Key_2;
		}
		case 2:
		{
			return FPMGameplayTags::Get().InputTag_Key_3;
		}
		case 3:
		{
			return FPMGameplayTags::Get().InputTag_Key_4;
		}
		default:
			break;
	}

	return FGameplayTag::EmptyTag;
}
