#include "MTable_NPC.h"
#include "Character/NPC/MNpcDefinition.h"
#include "Character/NPC/MNpcBase.h"

#if WITH_EDITOR
void FMTable_NPC::OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName)
{
	Super::OnDataTableChanged(InDataTable, InRowName);

	UMNpcDefinition* DefinitionCDO = Cast<UMNpcDefinition>(Definition.GetDefaultObject());
	if (DefinitionCDO && DefinitionCDO->NpcClass)
	{
		AMNpcBase* NpcCDO = DefinitionCDO->NpcClass->GetDefaultObject<AMNpcBase>();
		if (NpcCDO)
		{
			NpcCDO->Modify();
			NpcCDO->SetNpcRowId(RowId);
		}
	}
}
#endif
