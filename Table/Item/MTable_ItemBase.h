#pragma once

#include "CoreMinimal.h"
#include "Table/MTable_TableBase.h"
#include "Templates/SubclassOf.h"
#include "MTable_ItemBase.generated.h"

class UTexture;
class UPMInventoryItemDefinition;

UENUM(BlueprintType)
enum class EMItemIdType : uint8
{
	Equipment UMETA(DisplayName="Equipment"),
	Consumable UMETA(DisplayName = "Consumable"),
	Shop UMETA(DisplayName = "Shop"),
	Monster UMETA(DisplayName = "Monster"),

	None UMETA(DisplayName = "None"),
};

/**
 *
 */
USTRUCT(BlueprintType, Blueprintable)
struct FMTable_ItemBase : public FMTable_TableBase
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
};