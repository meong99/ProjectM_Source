#include "MTable_Shop.h"

void FMTable_Shop::OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName)
{
#if WITH_EDITOR
	ShopDefinition.RowId = RowId;
#endif
}
