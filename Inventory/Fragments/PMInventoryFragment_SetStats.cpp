// Fill out your copyright notice in the Description page of Project Settings.

#include "PMInventoryFragment_SetStats.h"
#include "Inventory/PMInventoryItemInstance.h"

void UPMInventoryFragment_SetStats::OnInstanceCreated(UPMInventoryItemInstance* Instance) const
{
	// iterating InitialItemStats and add stat tag to InventoryItemInstance
	for (const auto& InitialItemStat : InitialItemStats)
	{
		Instance->AddStatTagStack(InitialItemStat.Key, InitialItemStat.Value);
	}
}