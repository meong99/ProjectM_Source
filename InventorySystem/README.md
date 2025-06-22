# 데이터 관리 시스템
![image](https://github.com/user-attachments/assets/8172e4b0-cbbc-41ae-8047-d76d1f24afc0)


위 사진은 데이터 관리 시스템의 흐름을 나타내고 각 흐름별 주요 코드는 아래에 추가되어있습니다.


<details>
  <summary>클릭해서 데이터테이블 에셋 로드 및 파싱 코드 열기</summary> 

  ```cpp
  void UMDataTableManager::LoadDataTables()
  {
      UPMAssetManager& AssetManager = UPMAssetManager::Get();
      check(UPMAssetManager::IsInitialized());

      FPrimaryAssetType AssetType{ UMTableAsset::StaticClass()->GetFName() };
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
```
</details>
<details>
  <summary>클릭해서 데이터 요청 및 탐색 코드 열기</summary> 

  ```cpp
//데이터 요청
UPMInventoryItemDefinition* UPMInventoryManagerComponent::GetItemDefCDO(const int32 ItemRowId)
{
	UPMInventoryItemDefinition* ItemCDO = nullptr;

	UMDataTableManager* TableManager = GEngine->GetEngineSubsystem<UMDataTableManager>();
	if (TableManager)
	{
		ItemCDO = UMDataTableManager::GetDefinitionObject<UPMInventoryItemDefinition>(this, ItemRowId);
	}

	return ItemCDO;
}


// 데이터 탐색
template<class T>
T* UMDataTableManager::GetDefinitionObject(UObject* WorldContext, const int32 RowId)
{
	return DuplicateObject<T>(GetDefinitionDefaultObject<T>(WorldContext, RowId), WorldContext);
}

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
  ```
</details>
