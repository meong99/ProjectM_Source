#pragma once

#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "MMonsterSpawner.generated.h"

class UMMonsterDefinition;
class AMMonsterBase;
class USplineComponent;
class UBillboardComponent;

UCLASS()
class PROJECTM_API AMMonsterSpawner : public AActor
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	AMMonsterSpawner(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds);

	/*
* Member Functions
*/
public:
	void OnDeadMonster(AMMonsterBase* DeadMonster);

protected:
	void SpawnMonster();

/*
* Member Variables
*/
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster")
	FGameplayTag SearchTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster")
	UMMonsterDefinition* MonsterDefinition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster", meta = ())
	int32 MonsterRowId = INDEX_NONE;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster")
	TObjectPtr<UBillboardComponent> BillboardComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster")
	USplineComponent* SplineComponent;

	UPROPERTY()
	TArray<AMMonsterBase*> SpawnedMonsters;

	float SpawnTimer = 0.0f;
};