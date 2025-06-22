#pragma once

#include "CoreMinimal.h"
#include "Table/MTable_TableBase.h"
#include "MTable_DialogueTable.generated.h"

/**
 *
 */
USTRUCT(BlueprintType, Blueprintable)
struct FMTable_DialogueTable : public FMTable_TableBase
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:

/*
* Member Functions
*/
public:

/*
* Member Variables
*/
public:
	UPROPERTY(EditDefaultsOnly, Category = "ProjectM")
	TArray<FText> DialogueArray;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectM")
	FText Callback1_TextName;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectM")
	FText Callback2_TextName;
};