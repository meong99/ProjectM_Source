#pragma once

#include "CoreMinimal.h"
#include "Table/MTable_TableBase.h"
#include "GameplayTagContainer.h"
#include "MTable_MapTable.generated.h"

class UPMUserFacingExperienceDefinition;

/**
 *
 */
USTRUCT(BlueprintType, Blueprintable)
struct FMTable_MapTable : public FMTable_TableBase
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
	UPROPERTY(EditDefaultsOnly, meta=(), Category = "ProjectM")
	FGameplayTag LevelTag;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectM")
	FString Ip;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectM")
	TObjectPtr<UPMUserFacingExperienceDefinition> UFED;
};