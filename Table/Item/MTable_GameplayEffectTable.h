#pragma once

#include "CoreMinimal.h"
#include "Table/MTable_TableBase.h"
#include "MTable_GameplayEffectTable.generated.h"

class UGameplayEffect;

/**
 *
 */
USTRUCT(BlueprintType, Blueprintable)
struct FMTable_GameplayEffectTable : public FMTable_TableBase
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectM")
	TSubclassOf<UGameplayEffect> GameplayEffect;
};