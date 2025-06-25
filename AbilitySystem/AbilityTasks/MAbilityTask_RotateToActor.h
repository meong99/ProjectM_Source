#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "MAbilityTask_RotateToActor.generated.h"

class AActor;

DECLARE_DELEGATE_OneParam(FOnHit, const TArray<FHitResult>& HitResults);

/**
 *
 */
UCLASS()
class PROJECTM_API UMAbilityTask_RotateToActor : public UAbilityTask
{
	GENERATED_BODY()
/*
* Overrided Functions
*/
public:
	static UMAbilityTask_RotateToActor* CreateRotateToActorTask(
		UGameplayAbility* InOwningAbility,
		const AActor* InTargetActor,
		float AngleTolerance = 5.0f);

protected:
    virtual void TickTask(float DeltaTime) override;
    virtual void Activate() override;
    virtual void OnDestroy(bool bInOwnerFinished) override;

/*
* Member Functions
*/
protected:
	void CheckAlignmentAndRotate();
/*
* Member Variables
*/
public:
	UPROPERTY()
	UGameplayAbility* OwningAbility;

	UPROPERTY()
	const AActor* TargetActor;

	UPROPERTY()
	float ToleranceDegrees;

	UPROPERTY()
	bool bDone;
};