#include "MQuestComponentBase.h"
#include "System/MDataTableManager.h"
#include "Engine/Engine.h"
#include "Table/MTable_QuestTable.h"
#include "Definitions/MQuestDefinition.h"

UMQuestComponentBase::UMQuestComponentBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
}

void UMQuestComponentBase::InitializeComponent()
{
	Super::InitializeComponent();

	InitQuestData();
}

void UMQuestComponentBase::InitQuestData()
{
	UMDataTableManager* TableManager = GEngine->GetEngineSubsystem<UMDataTableManager>();
	if (TableManager)
	{
		const UDataTable* QuestTable = TableManager->GetDataTable(EMTableKey::Quest);
		if (QuestTable)
		{
			const TArray<FName>& Names = QuestTable->GetRowNames();
			for (const FName& Name : Names)
			{
				FMTable_QuestTable* RowData = QuestTable->FindRow<FMTable_QuestTable>(Name, Name.ToString());
				if (RowData && RowData->Definition)
				{
					QuestDatas.Add(RowData->RowId, DuplicateObject(RowData->Definition->GetDefaultObject<UMQuestDefinition>(), this));
				}
			}
		}
	}
}
