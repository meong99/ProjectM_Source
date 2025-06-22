#pragma once

#include "CoreMinimal.h"
#include "Table/MTable_TableBase.h"
#include "GameplayTagContainer.h"
#include "MTable_LevelTable.generated.h"

class UNiagaraSystem;
class USoundCue;
class UMGameplayEffectSet;

/**
 *
 */
USTRUCT(BlueprintType, Blueprintable)
struct FMTable_LevelTable : public FMTable_TableBase
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
	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=0.f), Category = "ProjectM")
	int64	MaxExperiencePoint = 1;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectM")
	TObjectPtr<UNiagaraSystem> LevelUpParticle;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectM")
	TObjectPtr<USoundCue> LevelUpSound;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectM")
	TSoftObjectPtr<UMGameplayEffectSet> EffectSet;
};