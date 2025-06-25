#include "PMQuickBarComponent.h"
#include "PMEquipmentManagerComponent.h"
#include "Inventory/PMInventoryItemInstance.h"
#include "Net/UnrealNetwork.h"
#include "Item/Equipment/MEquipableItemInstance.h"

UPMQuickBarComponent::UPMQuickBarComponent(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/) : Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UPMQuickBarComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Slots);
	DOREPLIFETIME(ThisClass, ActiveSlotIndex);
}

void UPMQuickBarComponent::BeginPlay()
{
}

void UPMQuickBarComponent::AddItemToSlot(const int32 SlotIndex, UPMInventoryItemInstance* Item)
{
}

void UPMQuickBarComponent::SetActiveSlotIndex(const int32 NewIndex)
{
}

UPMEquipmentManagerComponent* UPMQuickBarComponent::FindEquipmentManager() const
{
	return nullptr;
}

void UPMQuickBarComponent::UnequipItemInSlot()
{
}

void UPMQuickBarComponent::EquipItemInSlot()
{
}

const UPMInventoryItemInstance* UPMQuickBarComponent::GetEquippedItemInstance() const
{
	return nullptr;
}

void UPMQuickBarComponent::OnRep_Slots()
{
}

void UPMQuickBarComponent::OnRep_ActiveSlotIndex()
{
}
