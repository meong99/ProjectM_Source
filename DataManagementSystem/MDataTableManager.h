// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "Table/MTableAsset.h"
#include "Templates/SubclassOf.h"
#include "Inventory/PMInventoryItemDefinition.h"
#include "Definitions/MDefinitionBase.h"
#include "Engine/Engine.h"
#include "MDataTableManager.generated.h"

class UDataTable;
class UMMonsterDefinition;

UENUM(BlueprintType)
enum class EMTableKey : uint8
{
	None,
	ConsumableItem,
	EquipmentItem,
	Shop,
	Monster,
	NPC,
	MapTable,
	Quest,
	Dialogue,
	MiscellaneousItem,
	LevelTable,
};

/**
 * 
 */
UCLASS()
class PROJECTM_API UMDataTableManager : public UEngineSubsystem
{
	GENERATED_BODY()
/*
* Overrided Function
*/
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

/*
* Member Functions
*/
public:
	template<class T>
	static const TSubclassOf<T>	GetDefinitionClass(const UObject* WorldContext, const int32 RowId);
	template<class T>
	static T*					GetDefinitionDefaultObject(UObject* WorldContext, const int32 RowId);
	template<class T>
	static T*					GetDefinitionObject(UObject* WorldContext, const int32 RowId);
	template<class T>
	static T*					GetTableRowData(const UObject* WorldContext, const int32 RowId);

	static int32 ChangeRowIdToKey(int32 RowId);
	static int32 ChangeRowIdToElementId(int32 RowId);
	static int32 MakeRowId(const int32 Key, const int32 ElementId);

	const UDataTable* GetDataTable(int32 RowId) const;
	const UDataTable* GetDataTable(EMTableKey Key) const;

	UFUNCTION(BlueprintCallable)
	UPMInventoryItemDefinition*	GetItemDefinition(int32 RowId) const;
	UFUNCTION(BlueprintCallable)
	UMMonsterDefinition*		GetMonsterDefinition(int32 RowId) const;

	int32 GetTableNum() const { return TableMap.Num(); }
private:
	void LoadDataTables();
	void OnLoadedDataTables();
	void ParseTableMap(UMTableAsset* TableAsset);

/*
* Member Variables
*/
private:
	TMap<int32/*table id*/, UDataTable*> TableMap;

	TArray<FPrimaryAssetId>	PrimaryAssetIdList;
};

template<class T>
T* UMDataTableManager::GetDefinitionDefaultObject(UObject* WorldContext, const int32 RowId)
{
	UMDataTableManager* TableManager = GEngine->GetEngineSubsystem<UMDataTableManager>();

	if (TableManager)
	{
		const UDataTable* Table = TableManager->GetDataTable(RowId);
		if (Table)
		{
			int32 ElementIndex = UMDataTableManager::ChangeRowIdToElementId(RowId) - 1;
			const TArray<FName>& Names = Table->GetRowNames();
			if (Names.IsValidIndex(ElementIndex))
			{
				FMTable_TableBase* RowData = Table->FindRow<FMTable_TableBase>(Names[ElementIndex], Names[ElementIndex].ToString());
				if (RowData && RowData->Definition)
				{
					return RowData->Definition->GetDefaultObject<T>();
				}
			}
		}
	}

	return nullptr;
}

template<class T>
T* UMDataTableManager::GetDefinitionObject(UObject* WorldContext, const int32 RowId)
{
	return DuplicateObject<T>(GetDefinitionDefaultObject<T>(WorldContext, RowId), WorldContext);
}

template<class T>
T* UMDataTableManager::GetTableRowData(const UObject* WorldContext, const int32 RowId)
{
	UMDataTableManager* TableManager = GEngine->GetEngineSubsystem<UMDataTableManager>();

	if (TableManager)
	{
		const UDataTable* Table = TableManager->GetDataTable(RowId);
		if (Table)
		{
			int32 ElementIndex = UMDataTableManager::ChangeRowIdToElementId(RowId) - 1;
			const TArray<FName>& Names = Table->GetRowNames();
			if (Names.IsValidIndex(ElementIndex))
			{
				return Table->FindRow<T>(Names[ElementIndex], Names[ElementIndex].ToString());
			}
		}
	}

	return nullptr;
}

template<class T>
const TSubclassOf<T> UMDataTableManager::GetDefinitionClass(const UObject* WorldContext, const int32 RowId)
{
	UMDataTableManager* TableManager = GEngine->GetEngineSubsystem<UMDataTableManager>();

	if (TableManager)
	{
		const UDataTable* Table = TableManager->GetDataTable(RowId);
		if (Table)
		{
			int32 ElementIndex = UMDataTableManager::ChangeRowIdToElementId(RowId) - 1;
			const TArray<FName>& Names = Table->GetRowNames();
			if (Names.IsValidIndex(ElementIndex))
			{
				FMTable_TableBase* RowData = Table->FindRow<FMTable_TableBase>(Names[ElementIndex], Names[ElementIndex].ToString());
				if (RowData && RowData->Definition)
				{
					return TSubclassOf<T>(RowData->Definition.Get());
				}
			}
		}
	}

	return nullptr;
}
