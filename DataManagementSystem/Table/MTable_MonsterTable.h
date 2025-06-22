#pragma once

#include "CoreMinimal.h"
#include "MTable_TableBase.h"
#include "Templates/SubclassOf.h"
#include "MTable_MonsterTable.generated.h"

class UMMonsterDefinition;

/**
 *
 */
USTRUCT(BlueprintType, Blueprintable)
struct FMTable_MonsterTable : public FMTable_TableBase
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
	/*
	* Member Variables
	*/
public:
};