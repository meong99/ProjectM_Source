// Fill out your copyright notice in the Description page of Project Settings.

#include "System/MDataTableManager.h"
#include "DataAssets/PMAssetManager.h"
#include "Engine/Engine.h"
#include "Table/Item/MTable_ConsumableItem.h"
#include "Misc/MessageDialog.h"
#include "Table/Item/MTable_ItemBase.h"
#include "Table/MTable_TableBase.h"
#include "Util/MGameplayStatics.h"
#include "Table/MTable_MonsterTable.h"
#include "Definitions/MMonsterDefinition.h"

void UMDataTableManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	UAssetManager::CallOrRegister_OnCompletedInitialScan(FSimpleMulticastDelegate::FDelegate::CreateLambda(
		[this]()->void
		{
			LoadDataTables();
		})
	);
}

const UDataTable* UMDataTableManager::GetDataTable(int32 RowId) const
{
	int32 Key = ChangeRowIdToKey(RowId);
	return TableMap.FindRef(Key);
}

const UDataTable* UMDataTableManager::GetDataTable(EMTableKey Key) const
{
	return TableMap.FindRef((int32)Key);
}

UPMInventoryItemDefinition* UMDataTableManager::GetItemDefinition(int32 RowId) const
{
	const TSubclassOf<UPMInventoryItemDefinition>& ItemDefinition = GetDefinitionClass<UPMInventoryItemDefinition>(this, RowId);
	if (ItemDefinition)
	{
		return Cast<UPMInventoryItemDefinition>(DuplicateObject(ItemDefinition->GetDefaultObject<UPMInventoryItemDefinition>(), GetTransientPackage()));
	}
	
	return nullptr;
}

UMMonsterDefinition* UMDataTableManager::GetMonsterDefinition(int32 RowId) const
{
	const TSubclassOf<UMMonsterDefinition>& MonsterDefinition = GetDefinitionClass<UMMonsterDefinition>(this, RowId);
	if (MonsterDefinition)
	{
		return Cast<UMMonsterDefinition>(DuplicateObject(MonsterDefinition->GetDefaultObject<UMMonsterDefinition>(), GetTransientPackage()));
	}

	return nullptr;
}

int32 UMDataTableManager::ChangeRowIdToKey(int32 RowId)
{
	int32 TableId = RowId / 100000;
	return TableId;
}

int32 UMDataTableManager::ChangeRowIdToElementId(int32 RowId)
{
	int32 ElementId = INDEX_NONE;

	if (!(RowId / 100000))
	{
		MCHAE_ERROR("RowId is not valid!!!!!id = % d", RowId);
		return ElementId;
	}

	ElementId = RowId % 100000;

	return ElementId;
}

int32 UMDataTableManager::MakeRowId(const int32 Key, const int32 ElementId)
{
	if (ElementId > 99999)
	{
		ensure(false);
		return -1;
	}

	int32 Ret = Key * 100000 + ElementId;

	return Ret + ElementId;
}

void UMDataTableManager::LoadDataTables()
{
	UPMAssetManager& AssetManager = UPMAssetManager::Get();
	check(UPMAssetManager::IsInitialized());

	FPrimaryAssetType		AssetType{ UMTableAsset::StaticClass()->GetFName() };
	AssetManager.GetPrimaryAssetIdList(AssetType, PrimaryAssetIdList);

	TArray<FSoftObjectPath> Paths;
	AssetManager.GetPrimaryAssetPathList(AssetType, Paths);
	AssetManager.GetStreamableManager().RequestSyncLoad(Paths);
	OnLoadedDataTables();
}

void UMDataTableManager::OnLoadedDataTables()
{
	UPMAssetManager& AssetManager = UPMAssetManager::Get();

	for (const FPrimaryAssetId& AssetId : PrimaryAssetIdList)
	{
		UMTableAsset* TableAsset = AssetManager.GetPrimaryAssetObject<UMTableAsset>(AssetId);

		if (IsValid(TableAsset))
		{
			ParseTableMap(TableAsset);
		}
		else
		{
			MCHAE_ERROR("Can't Get TableAsset");
		}
	}
}

void UMDataTableManager::ParseTableMap(UMTableAsset* TableAsset)
{
	for (const FMTableDefinition& TableDefinition : TableAsset->TableDefinitions)
	{
		UDataTable* DataTable = TableDefinition.Table;
		if (DataTable == nullptr)
		{
			UMGameplayStatics::ShowErrorOrLog(TEXT("Table Data Error!\n 비어있는 데이터테이블이 있습니다!"));
			continue;
		}
		TArray<FMTable_TableBase*> TableArray;
		DataTable->GetAllRows<FMTable_TableBase>(TEXT("UMDataTableManager::ParseTableMap"), TableArray);

		int32 Key = INDEX_NONE;
		TSet<int32> ElementValidation;
		for (const FMTable_TableBase* TableRow : TableArray)
		{
			if (!TableRow)
			{
				UMGameplayStatics::ShowErrorOrLog(
					FString::Printf(TEXT("Table Data Error! - TableName : \"%s\"\n 테이블은 모두 FMTable_TableBase의 하위여야합니다"), *DataTable->GetName()));
				continue;
			}
			if (!TableRow->IsValidId())
			{
				UMGameplayStatics::ShowErrorOrLog(
					FString::Printf(TEXT("Table Data Error! - TableName : \"%s\"\n테이블 속성의 ID가 잘못됐습니다!\n속성의 ID는 xyyyyy의 형태여야합니다! \n Error RowId = %d"), *DataTable->GetName(), TableRow->RowId));
				continue;
			}
			if (ElementValidation.Find(TableRow->GetElementId()))
			{
				UMGameplayStatics::ShowErrorOrLog(
						FString::Printf(TEXT("Table Data Error! - TableName : \"%s\"\n 테이블 RowId에 중복이 있습니다! 모든 테이블 RowId는 상이해야합니다! \n Error RowId = %d"), *DataTable->GetName(), TableRow->RowId));
				continue;
			}

			ElementValidation.Add(TableRow->GetElementId());

			if (Key == INDEX_NONE)
			{
				Key = TableRow->GetKey();
			}
			else if (Key != TableRow->GetKey())
			{
				UMGameplayStatics::ShowErrorOrLog(
					FString::Printf(TEXT("Table Data Error! - TableName : \"%s\"\n 테이블 내에 키가 다른 RowId가 있습니다! 테이블 내의 모든 Row의 Key는 동일해야합니다! \n ErrorRowId = %d"), *DataTable->GetName(), TableRow->RowId));
				continue;
			}
		}

		if (Key != INDEX_NONE)
		{
			if (TableMap.FindRef(Key))
			{
				UMGameplayStatics::ShowErrorOrLog(
							FString::Printf(TEXT("Table Data Error! - TableName : \"%s\"\n 테이블 키의 중복이 있습니다! 각 테이블간의 키는 상이해야합니다!"), *DataTable->GetName()));
			}
			else
			{
				TableMap.Emplace(Key, DataTable);
			}
		}
	}
}
