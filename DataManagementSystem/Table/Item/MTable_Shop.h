#pragma once

#include "CoreMinimal.h"
#include "Table/MTable_TableBase.h"
#include "Types/MShopTypes.h"
#include "MTable_Shop.generated.h"

/**
 *
 */
USTRUCT(BlueprintType, Blueprintable)
struct FMTable_Shop : public FMTable_TableBase
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
	UPROPERTY(EditDefaultsOnly)
	FMShopDefinition ShopDefinition;
};