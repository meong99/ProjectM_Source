#include "PMEquipmentManagerComponent.h"
#include "Engine/Engine.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/PMAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "System/MDataTableManager.h"
#include "Table/Item/MTable_EquipmentItem.h"
#include "Inventory/PMInventoryManagerComponent.h"
#include "PMQuickBarComponent.h"
#include "Item/Equipment/MEquipableItemInstance.h"
#include "Item/Equipment/MEquipmentItemDefinition.h"
#include "AbilitySystem/PMAbilitySet.h"
#include "Player/PMPlayerControllerBase.h"
#include "Item/Equipment/MEquipmentItemInstance.h"
#include "Player/PMPlayerState.h"
#include "Character/PMPawnData.h"

/*
* UPMEquipmentManagerComponent
*/
UPMEquipmentManagerComponent::UPMEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquippedItemList(this)
{
	SetIsReplicatedByDefault(true);
}

void UPMEquipmentManagerComponent::OnSetNewPawn(APawn* NewPawn, APawn* OldPawn)
{
	if (IsValid(NewPawn))
	{
		if (!IsEquipped(EMEquipmentItemType::Weapon))
		{
			bIsInitialized = true;
			EquipDefaultWeapon();
		}
		else
		{
			UnequipAllItems();
			EquipAllItems();
		}
	}
}

void UPMEquipmentManagerComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);

	UnequipAllItems();
}

void UPMEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquippedItemList);
}

void UPMEquipmentManagerComponent::EquipItem(const int32 ItemRowId)
{
	const UMEquipmentItemDefinition* EquipDef = UMDataTableManager::GetDefinitionObject<UMEquipmentItemDefinition>(this, ItemRowId);
	if (EquipDef)
	{
		if (FindEquippedItemInstance(EquipDef->EquipmentItemType))
		{
			UnequipItem(EquipDef->EquipmentItemType);
		}

		EquipItem_Impl(EquipDef);
	}
	else
	{
		ensure(false);
		MCHAE_ERROR("장비 장착 중 Definition을 찾지 못 함");
	}
}

void UPMEquipmentManagerComponent::UnequipItem(EMEquipmentItemType EquipmentItemType)
{
	UMEquipmentItemInstance* UnEquippedItem = FindEquippedItemInstance(EquipmentItemType);

	if (UnEquippedItem)
	{
		AController* Controller = GetPlayerController<AController>();
		UPMInventoryManagerComponent* InvenManager = Controller ? Controller->FindComponentByClass<UPMInventoryManagerComponent>() : nullptr;
		if (InvenManager)
		{
			FMItemRequest Request;
			Request.SetItemRequest(EMItemRequestType::ReturnItemToInventory, UnEquippedItem->ItemRowId, 1, UnEquippedItem->ItemHandle);
			InvenManager->RequestItemChange(Request);
		}
		UnEquippedItem->OnUnequipped();
		TakeAbilitiesFromAsc(UnEquippedItem);
		RemoveEntry(EquipmentItemType);
		if (IsUsingRegisteredSubObjectList())
		{
			RemoveReplicatedSubObject(UnEquippedItem);
		}
	}
}

UMEquipmentItemInstance* UPMEquipmentManagerComponent::FindEquippedItemInstance(EMEquipmentItemType EquipmentItemType)
{
	FMEquipmentItemEntry* Entry = FindEntry(EquipmentItemType);
	UMEquipmentItemInstance* Instance = nullptr;
	if (Entry)
	{
		Instance = Entry->Instance;
	}

	return Instance;
}

FMEquipmentItemEntry* UPMEquipmentManagerComponent::FindEntry(EMEquipmentItemType EquipmentItemType)
{
	FMItemHandle Handle = EquippedEntryMap.FindRef(EquipmentItemType);
	return EquippedItemList.FindEntry(Handle);
}

void UPMEquipmentManagerComponent::EquipItem_Impl(const UMEquipmentItemDefinition* EquipDef)
{
	FMItemHandle Handle = EquippedItemList.AddEntry(EquipDef->GetClass(), 1);
	FMEquipmentItemEntry* Entry = EquippedItemList.FindEntry(Handle);
	if (Entry && Entry->Instance)
	{
		GiveAbilities(EquipDef, Entry->Instance);
		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && Entry->Instance)
		{
			AddReplicatedSubObject(Entry->Instance);
		}

		EquippedEntryMap.Add(Entry->Instance->EquipmentItemType, Entry->GetItemHandle());
		Entry->Instance->OnEquipped();
	}
}

void UPMEquipmentManagerComponent::RemoveEntry(EMEquipmentItemType EquipmentItemType)
{
	FMItemHandle Handle = EquippedEntryMap.FindRef(EquipmentItemType);
	EquippedItemList.RemoveEntry(Handle);
}

void UPMEquipmentManagerComponent::GiveAbilities(const UMEquipmentItemDefinition* EquipDef, UMEquipmentItemInstance* Instance)
{
	UPMAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (ASC == nullptr)
	{
		ensure(false);
		MCHAE_ERROR("Can't access to abilitysystem when equip item. So item equip sequence will be canceled.");
		return;
	}

	FMAbilitySet_GrantedHandles TempGrantedHandles;
	for (const UPMAbilitySet* AbilitySet : EquipDef->AbilitySetsToGrant)
	{
		AbilitySet->GiveToAbilitySystem(ASC, &TempGrantedHandles, Instance, EquipDef->RowId);
	}

	GrantedHandles.Add(Instance->ItemHandle.ItemUid, TempGrantedHandles);
}

void UPMEquipmentManagerComponent::TakeAbilitiesFromAsc(UMEquipmentItemInstance* Instance)
{
	UPMAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (ASC == nullptr)
	{
		return;
	}

	FMAbilitySet_GrantedHandles TempGrantedHandles;
	GrantedHandles.RemoveAndCopyValue(Instance->ItemHandle.ItemUid, TempGrantedHandles);
	TempGrantedHandles.TakeFromAbilitySystem(ASC);
}

void UPMEquipmentManagerComponent::EquipAllItems()
{
	for (FMEquipmentItemEntry& Entry : EquippedItemList.Entries)
	{
		UMEquipmentItemInstance* Instance = Entry.Instance;
		if (IsValid(Instance))
		{
			const UMEquipmentItemDefinition* ItemCDO = Instance->ItemDef->GetDefaultObject<UMEquipmentItemDefinition>();
			if (ItemCDO)
			{
				Instance->OnEquipped();
				GiveAbilities(ItemCDO, Instance);
			}
		}
	}
}

void UPMEquipmentManagerComponent::UnequipAllItems()
{
	for (FMEquipmentItemEntry& Entry : EquippedItemList.Entries)
	{
		UMEquipmentItemInstance* Instance = Entry.Instance;
		if (IsValid(Instance))
		{
			Instance->OnUnequipped();
			TakeAbilitiesFromAsc(Instance);
		}
	}
}

void UPMEquipmentManagerComponent::EquipDefaultWeapon()
{
	APMPlayerControllerBase* Controller = GetPlayerController<APMPlayerControllerBase>();
	APMPlayerState* PlayerState = Controller ? Controller->GetPlayerState() : nullptr;
	if (PlayerState)
	{
		const UPMPawnData* PawnData = PlayerState->GetPawnData();
		if (PawnData)
		{
			EquipItem(PawnData->DefaultEquipmentRowId);
		}
	}
}

UPMAbilitySystemComponent* UPMEquipmentManagerComponent::GetAbilitySystemComponent() const
{
	APMPlayerControllerBase* Controller = GetPlayerController<APMPlayerControllerBase>();
	if (Controller)
	{
		return Controller->GetAbilitySystemComponent();
	}

	return nullptr;
}
