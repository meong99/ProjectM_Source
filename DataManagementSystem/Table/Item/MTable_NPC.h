#pragma once

#include "CoreMinimal.h"
#include "Table/MTable_TableBase.h"
#include "MTable_NPC.generated.h"

class UMNpcDefinition;

/**
 *
 */
USTRUCT(BlueprintType, Blueprintable)
struct FMTable_NPC : public FMTable_TableBase
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
#if WITH_EDITOR
	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override;
#endif

/*
* Member Functions
*/
public:

/*
* Member Variables
*/
public:
};