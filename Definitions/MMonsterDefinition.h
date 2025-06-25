// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Character/Monster/MMonsterTypes.h"
#include "MDefinitionBase.h"
#include "Types/MTradeType.h"
#include "MMonsterDefinition.generated.h"

class UMInteractiveAction_OverlapActionBase;
class UMInteractiveAction_OnInteractionBase;
class UAnimMontage;
class UPMAbilitySet;
class AMRangeIndicatorActor;

/**
 *
 */
UCLASS(Blueprintable)
class PROJECTM_API UMMonsterDefinition : public UMDefinitionBase
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UMMonsterDefinition();

	FMTradeRequest CreateTradeRequest() const;

/*
* Member Functions
*/
public:
	UFUNCTION(BlueprintCallable)
	int32			GetMonsterHp() const { return MonsterInfo.MonsterHp; }

	UFUNCTION(BlueprintCallable)
	int32			GetMonsterReward() const { return MonsterInfo.MonsterReward; }

	UFUNCTION(BlueprintCallable)
	int32			GetMonsterSpawningCooldown() const { return MonsterSpawningCooldown; }

	UFUNCTION(BlueprintCallable)
	int32			GetMaximumSpawnNum() const { return MaximumSpawnNum; }

	UFUNCTION(BlueprintCallable)
	const FName&	GetMonsterName() const { return MonsterInfo.MonsterName; }

	UFUNCTION(BlueprintCallable)
	const FMMonsterInfo&		GetMonsterInfo() const { return MonsterInfo; }

	UFUNCTION(BlueprintCallable)
	TSubclassOf<AMMonsterBase>	GetMonsterClass() const { return MonsterInfo.MonsterClass; }

	UFUNCTION(BlueprintCallable)
	UBehaviorTree*				GetBehaviorTree() const { return MonsterInfo.BehaviorTree; }

	UFUNCTION(BlueprintCallable)
	const TArray<FMDropInfo>&	GetItemDropTable() const { return MonsterInfo.ItemDropTable; }

/*
* Member Variables
*/
public:
	UPROPERTY(EditDefaultsOnly)
	FMMonsterInfo MonsterInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectM")
	FMMonsterCombatInfo MonsterCombatInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectM")
	FMMontageInfo DeathAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectM | Abilities")
	TArray<TObjectPtr<UPMAbilitySet>> AbilitySets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 1, ClampMax = 5000), Category = "ProjectM")
	int32 MonsterSpawningCooldown = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 1, ClampMax = 10), Category = "ProjectM")
	int32 MaximumSpawnNum = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "ProjectM")
	TArray<TObjectPtr<UMInteractiveAction_OverlapActionBase>> Action_OnBeginOverlap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "ProjectM")
	TArray<TObjectPtr<UMInteractiveAction_OnInteractionBase>> Action_OnInteract;
};
