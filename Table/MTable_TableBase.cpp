#include "MTable_TableBase.h"
#include "Definitions/MDefinitionBase.h"
#include "System/MDataTableManager.h"

void FMTable_TableBase::OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName)
{
#if WITH_EDITOR
	UMDefinitionBase* DefinitionCDO = Cast<UMDefinitionBase>(Definition.GetDefaultObject());
	if (DefinitionCDO)
	{
		DefinitionCDO->Modify();
		DefinitionCDO->RowId = RowId;
	}
#endif
}

bool FMTable_TableBase::IsValidId() const
{
	bool bIsValid = RowId != INDEX_NONE;

	int32 TableId = GetKey();
	bIsValid &= TableId >= 1;

	int32 ElementId = GetElementId();
	bIsValid &= ElementId >= 1 && ElementId <= 99999;

	return bIsValid;
}

int32 FMTable_TableBase::GetKey() const
{
	return UMDataTableManager::ChangeRowIdToKey(RowId);
}

int32 FMTable_TableBase::GetElementId() const
{
	return UMDataTableManager::ChangeRowIdToElementId(RowId);
}
