#include "MTable_MonsterTable.h"
#include "Definitions/MMonsterDefinition.h"
#include "Character/Monster/MMonsterBase.h"

void FMTable_MonsterTable::OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName)
{
#if WITH_EDITOR
	Super::OnDataTableChanged(InDataTable, InRowName);

	UMMonsterDefinition* DefinitionCDO = Cast<UMMonsterDefinition>(Definition.GetDefaultObject());
	if (DefinitionCDO && DefinitionCDO->MonsterInfo.MonsterClass)
	{
		AMMonsterBase* MonsterCDO = DefinitionCDO->MonsterInfo.MonsterClass->GetDefaultObject<AMMonsterBase>();
		if (MonsterCDO)
		{
			MonsterCDO->Modify();
			MonsterCDO->SetMonsterRowId(RowId);
		}
	}
#endif
}
