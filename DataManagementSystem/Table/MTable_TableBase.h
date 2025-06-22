#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Templates/SubclassOf.h"
#include "MTable_TableBase.generated.h"

class UMDefinitionBase;

/**
 *
 */
USTRUCT(BlueprintType, Blueprintable)
struct FMTable_TableBase : public FTableRowBase
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override;

	/*
* Member Functions
*/
public:
	template<class T>
	TSubclassOf<T> GetDefinition() const { return Definition.Get(); }
	bool	IsValidId() const;
	int32	GetKey() const;
	int32	GetElementId() const;
/*
* Member Variables
*/
public:
	// x...xyyyyyy
	// x = TableId(1~nnn...), y = ElementId(00001~99999)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectM")
	int32 RowId = INDEX_NONE;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectM")
	TSubclassOf<UMDefinitionBase> Definition;
};
