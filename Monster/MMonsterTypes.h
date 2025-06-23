#pragma once

#include "Templates/SubclassOf.h"
#include "Types/MAnimTypes.h"
#include "MMonsterTypes.generated.h"

class UBehaviorTree;
class AMMonsterBase;
class UGameplayEffect;
class UMGameplayEffectSet;

USTRUCT(BlueprintType, Blueprintable)
struct FMDropInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ItemId = INDEX_NONE;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=1))
	int32 ItemCount = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0.0f, ClampMax=1.0f))
	float DropChance = 0.0f;
};

USTRUCT(BlueprintType, Blueprintable)
struct FMMonsterInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AMMonsterBase> MonsterClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectM")
	TObjectPtr<UMGameplayEffectSet> EffectSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName MonsterName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 1, ClampMax = 2147483647))
	int32 MonsterHp = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0, ClampMax = 2147483647))
	int32 MonsterReward = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int64 ExperiencePoint = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MonsterMaxSpeed = 300.f;

	// Key = ItemId, Value = 확률
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FMDropInfo> ItemDropTable;
};

USTRUCT(BlueprintType, Blueprintable)
struct FMMonsterCombatInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DefaultAttack MontageInfo")
	FMMontageInfo MontageInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageApplyEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MonsterDefaultAttackRange = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MonsterDefaultAttackRadius = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MonsterAttackPower = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MonsterDefensePower = 0.f;
};

