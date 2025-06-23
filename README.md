# 프로젝트 정보

- 엔진 버전  : 5.4.4
- 플레이 환경 : DedicatedServer or StandAlone, Windows
- 다운로드 링크 : [게임 다운로드 - 구글드라이브](https://drive.google.com/file/d/1PXUecAwcBvVVgHUpi_HPQFYBcRvfuSMG/view?usp=drive_link)
- 플레이 영상 : [ 인게임 소개 영상 - 유튜브](https://www.youtube.com/watch?v=pqBk47X7rvo&t=56s)

# 주요 구현 기능
<details>
  <summary>데이터 관리 시스템 (Click!)</summary>
  <img src="https://github.com/user-attachments/assets/9c9d54ca-f334-4994-9585-d1d6f0bc6bbd" width="600"/>

  기본적인 흐름은 위와 같으며 위 흐름 기반의 코드플로우는 다음과 같습니다.
  <ul>
  <details>
  <summary>데이터테이블 에셋 로드 및 파싱 코드 (플로우 1~2)</summary> 

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
</ul>
<ul>
<details>
  <summary>데이터 요청 및 탐색 코드 (플로우 3~5)</summary> 

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
</ul>
  주요 코드 링크
    <ul>
      <li><a href="https://github.com/meong99/ProjectM_Source/blob/main/DataManagementSystem/MDataTableManager.cpp">MDataTableManager.cpp</a></li>
      <li><a href="https://github.com/meong99/ProjectM_Source/blob/main/DataManagementSystem/MDataTableManager.h">MDataTableManager.h</a></li>
    </ul>
  <hr>
</details>


<details>
  <summary>UI관리 시스템 (Click!)</summary> 
  <img src="https://github.com/user-attachments/assets/fb5f65b8-5339-4840-8514-369bcb28a826" width="600"/>
  
  기본적인 흐름은 위와 같으며 위 흐름 기반의 코드플로우는 다음과 같습니다.
  <ul>
    <details>
    <summary>위젯 데이터 에셋 로드 (플로우 1)</summary> 

  ```cpp

void UPMExperienceManagerComponent::StartExperienceLoad()
{
	check(CurrentExperience);
	check(LoadState == EPMExperienceLoadState::Unloaded);

	LoadState = EPMExperienceLoadState::Loading;

	UPMAssetManager& AssetManager = UPMAssetManager::Get();

	TSet<FPrimaryAssetId> BundleAssetList;
	BundleAssetList.Add(CurrentExperience->GetPrimaryAssetId());

	for (const TObjectPtr<UPMExperienceActionSet>& ActionSet : CurrentExperience->GetActionSets())
	{
		if (ActionSet)
		{
			BundleAssetList.Add(ActionSet->GetPrimaryAssetId());
		}
	}

	TArray<FName> BundlesToLoad;
	const ENetMode OwnerNetMode = GetOwner()->GetNetMode();
	bool bLoadClient = GIsEditor || OwnerNetMode != NM_DedicatedServer;
	bool bLoadServer = GIsEditor || OwnerNetMode != NM_Client;
	if (bLoadClient)
	{
		BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateClient);
	}
	if (bLoadServer)
	{
		BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateServer);
	}

	FStreamableDelegate OnAssetsLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ThisClass::OnExperienceLoadComplete);
	TSharedPtr<FStreamableHandle> Handle = AssetManager.ChangeBundleStateForPrimaryAssets(
		BundleAssetList.Array(),
		BundlesToLoad,
		{},
		false,
		FStreamableDelegate(),
		FStreamableManager::AsyncLoadHighPriority
	);

	if (Handle.IsValid() == false || Handle->HasLoadCompleted())
	{
		FStreamableHandle::ExecuteDelegate(OnAssetsLoadedDelegate);
	}
	else
	{
		Handle->BindCompleteDelegate(OnAssetsLoadedDelegate);
		Handle->BindCancelDelegate(FStreamableDelegate::CreateLambda([OnAssetsLoadedDelegate]() -> void
		{
			OnAssetsLoadedDelegate.ExecuteIfBound();	
		}));
	}
}
```

   </details>
  </ul>
  <ul>
    <details>
    <summary>데이터 파싱, RootLayer 추가 (플로우 2~5)</summary> 

  ```cpp

void UGameFeatureAction_AddRegisterWidget::AddWidgetRegister(FGameFeatureStateChangeContext ChangeContext)
{
	const FPerContextData& ActiveData = ContextData.FindRef(ChangeContext);
	UGameInstance* GameInstance = ActiveData.GameInstance;
	UMViewportClient* ViewportClient = GameInstance ? Cast<UMViewportClient>(GameInstance->GetGameViewportClient()) : nullptr;

	if (ViewportClient)
	{
		for (FMWidgetRegisterHandle RegisterHandle : WidgetRegisterHandles)
		{
			UMWidgetRegister* WidgetRegister = RegisterHandle.WidgetRegister;
			if (IsValid(WidgetRegister) && WidgetRegister->RegisterTag.IsValid())
			{
				// 위젯 데이터 저장
				ViewportClient->AddWidgetRegister(RegisterHandle.WidgetRegister->RegisterTag, WidgetRegister);
			}

			//RootLayer 추가
			ViewportClient->ApplyWidgetLayout();

			if (bAddToLayer)
			{
				for (const auto& Iter : WidgetRegister->MappedWidgetData.WidgetData)
				{
					ViewportClient->AddWidgetToLayer(Iter.Key);
				}
			}
		}
	}
}


// 위젯 데이터 저장
void UMViewportClient::AddWidgetRegister(const FGameplayTag& RegisterTag, UMWidgetRegister* InWidgetRegister)
{
	if (RegisterTag.IsValid() && InWidgetRegister)
	{
		RemoveWidgetRegister(RegisterTag);
		WidgetRegisterMap.Emplace(RegisterTag, InWidgetRegister);
	}
}

// RootLayer 추가
void UMViewportClient::ApplyWidgetLayout()
{
	if (WidgetLayout)
	{
		if (!WidgetLayout->IsInViewport())
		{
			WidgetLayout->AddToViewport();
		}
		return;
	}

	UMWidgetRegister* WidgetRegister = GetWidgetRegister(FPMGameplayTags::Get().UI_Registry_Layout_DefaultLayout.RequestDirectParent());

	WidgetLayout = Cast<UMWidgetLayout>(GetWidgetInstance(FPMGameplayTags::Get().UI_Registry_Layout_DefaultLayout));
	if (!WidgetLayout)
	{
		MCHAE_ERROR("Can't find DefaultLayout!!!!!!");
		check(false);
		return;
	}

	if (!WidgetLayout->IsInitialized())
	{
		WidgetLayout->PreAddToLayer();
	}

	WidgetLayout->AddToViewport();
}

  ```
  </details>
  </ul>
    주요 코드 링크
    <ul>
      <li><a href="https://github.com/meong99/ProjectM_Source/blob/main/UIManagementSystem/MViewportClient.cpp">MViewportClient.cpp</a></li>
      <li><a href="https://github.com/meong99/ProjectM_Source/blob/main/UIManagementSystem/MViewportClient.h">MViewportClient.h</a></li>
    </ul>
  <hr>
</details>

<details>
  <summary>상호작용 시스템 (Click!)</summary>
  <img src="https://github.com/user-attachments/assets/1909fa9a-d359-4e96-807f-4744c5811215" width="600"/>

  기본적인 흐름은 위와 같으며 위 흐름 기반의 코드플로우는 다음과 같습니다.
    <ul>
      <details>
      <summary>OverlapAction 실행, 입력 대기 (플로우 2~3)</summary> 

```cpp
void UMInteractionComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor->GetLocalRole() == ENetRole::ROLE_SimulatedProxy)
	{
		return;
	}

	EnableOverlapAction(OtherActor);
}

void UMInteractionComponent::EnableOverlapAction(AActor* OtherActor)
{
	AMPlayerCharacterBase* Character = Cast<AMPlayerCharacterBase>(OtherActor);
	if (Character)
	{
		AActor* Owner = GetOwner();
		if (Owner && Owner->IsA(AMMonsterBase::StaticClass()))
		{
			Character->AddOverlappedMonster(Owner);
		}
		WeakOverlappedCharacter = Character;
	}

	if (!IsItServer())
	{
		//Overlap 액션 실행
		ActivateAllOverlapAction();
		SetComponentTickEnabled(true);

		BindDelegate();
	}
}

void UMInteractionComponent::BindDelegate()
{
	if (Action_OnInteract.Num() == 0)
	{
		return;
	}

	UPMInputComponent* InputComponent = GetInputComponent();
	if (InputComponent)
	{
		// 플레이어 입력 대기
		InputComponent->Delegate_InputActionMap.Emplace(FPMGameplayTags::Get().InputTag_Togle_Interaction, 
			FInputActionDelegate::FDelegate::CreateUObject(this, &ThisClass::OnInteract));
	}
}

```
  </details>
  </ul>
  <ul>
    <details>
    <summary>액션 리스트 출력, 선택, 실행 (플로우 5~7)</summary> 

```cpp
FReply UMInteractionListElementWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InteractiveAction)
	{
		InteractiveAction->ActivateAction();
	}

	return FReply::Handled();
}

void UMInteractiveAction_OnInteractionBase::ActivateAction()
{
	Super::ActivateAction();

	if (InteractionComponent)
	{
		InteractionComponent->OnActivatedInteractiveAction(this);
	}
}
```
  </details>
  </ul>
  주요 코드 링크
    <ul>
      <li><a href="https://github.com/meong99/ProjectM_Source/blob/main/InteractionSystem/MInteractionComponent.cpp">MInteractionComponent.cpp</a></li>
      <li><a href="https://github.com/meong99/ProjectM_Source/blob/main/InteractionSystem/MInteractionComponent.h">MInteractionComponent.h</a></li>
    </ul>
  <hr>
</details>

<details>
  <summary>인벤토리 시스템 (Click!)</summary>
  <img src="https://github.com/user-attachments/assets/cec6b30e-331b-44fd-8f4d-f2123abdcff9" width="600"/>

  기본적인 흐름은 위와 같으며 위 흐름 기반의 코드플로우는 다음과 같습니다.
    <ul>
    <details>
    <summary>요청 분석, 아이템 추가 (플로우 2~3)</summary> 

```cpp

FMItemHandle UPMInventoryManagerComponent::RequestItemChange(const FMItemRequest& ItemRequest)
{
	FMItemHandle Handle = FMItemHandle{};
	if (!HasAuthority())
	{
		ensure(false);
		return Handle;
	}

	UPMInventoryItemDefinition* ItemCDO = GetItemDefCDO(ItemRequest);
	if (!ItemCDO)
	{
		ensure(false);
		MCHAE_WARNING("ItemCDO is not valid");
		return Handle;
	}

	FPMInventoryItemList* ItemList = GetItemList(ItemCDO->ItemType);
	if (ItemList)
	{
		FPMInventoryEntry* Entry = ItemRequest.ItemHandle.IsValid() ? 
			ItemList->FindEntry(ItemRequest.ItemHandle) : ItemList->FindEntry(ItemCDO->GetClass());

		bool bCanChangeItemQuentity = ItemList->OwnedItemType != EMItemType::Equipment;
		bCanChangeItemQuentity &= (ItemRequest.RequestType == EMItemRequestType::AddItem || ItemRequest.RequestType == EMItemRequestType::RemoveItem);

		if (Entry && bCanChangeItemQuentity)
		{
			//아이템 갯수 변경
			Handle = Entry->GetItemHandle();
			ChangeItemQuantity(Entry->Instance, ItemRequest);
		}
		else
		{
			// 새로운 아이템 추가
			Handle = AddItemDefinition_Impl(ItemCDO->GetClass(), *ItemList, ItemRequest);
		}
	}

	return Handle;
}

```
  </details>
  </ul>
      <ul>
    <details>
    <summary>아이템 추가, 객체 생성 (플로우 3~6)</summary> 

```cpp

FMItemHandle UPMInventoryManagerComponent::AddItemDefinition_Impl(TSubclassOf<UPMInventoryItemDefinition> ItemDef, FPMInventoryItemList& ItemList, const FMItemRequest& ItemRequest)
{
	FMItemHandle Handle = FMItemHandle{};

	// 아이템 추가
	Handle = ItemList.AddEntry(ItemDef, ItemRequest.ItemQuentity);

	FPMInventoryEntry* Entry = ItemList.FindEntry(Handle);
	if (Entry == nullptr)
	{
		return FMItemHandle{};
	}

	UPMInventoryItemInstance* ItemInstance = Entry->Instance;

	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && ItemInstance)
	{
		AddReplicatedSubObject(ItemInstance);
	}

	ItemInstance->ItemResponse.SetItemResponse(ItemRequest, EMItemResponseType::TotallyNewItem, ItemInstance->GetItemQuentity(), ItemInstance->ItemHandle, ItemInstance);

	if (GetNetMode() == ENetMode::NM_Standalone)
	{
		Broadcast_OnItemIncreased(ItemInstance->ItemResponse);
	}

	return Handle;
}

// 아이템 객체 생성
FMItemHandle FPMInventoryItemList::AddEntry(TSubclassOf<UPMInventoryItemDefinition> ItemDef, int32 ItemQuentity)
{
	if (!ItemDef || !OwnerComponent)
	{
		MCHAE_WARNING("ItemDefinition is not valid");
		return {};
	}

	AActor* OwningActor = OwnerComponent->GetOwner();
	if (!OwningActor->HasAuthority())
	{
		MCHAE_WARNING("AddEntry only allowed on authority. You called on client");
		return {};
	}


	FPMInventoryEntry* NewEntry = MakeEntry(ItemDef, ItemQuentity);
	AddEntry_Impl(*NewEntry);

	return NewEntry->GetItemHandle();
}

```
  </details>
  </ul>

  주요 코드 링크
    <ul>
      <li><a href="https://github.com/meong99/ProjectM_Source/blob/main/InventorySystem/Inventory/PMInventoryManagerComponent.cpp">PMInventoryManagerComponent.cpp</a></li>
      <li><a href="https://github.com/meong99/ProjectM_Source/blob/main/InventorySystem/Inventory/PMInventoryManagerComponent.h">PMInventoryManagerComponent.h</a></li>
      <li><a href="https://github.com/meong99/ProjectM_Source/blob/main/InventorySystem/Inventory/PMInventoryItemList.cpp">PMInventoryItemList.cpp</a></li>
      <li><a href="https://github.com/meong99/ProjectM_Source/blob/main/InventorySystem/Inventory/PMInventoryItemList.h">PMInventoryItemList.h</a></li>
    </ul>
    
  <hr>
</details>

## 기타 기능
- 퀘스트
- 몬스터
- 전투
- 장비, 소비, 기타 아이템
- 퀵슬롯
- 길찾기
