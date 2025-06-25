#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "MAbilityTask_SphereTracer.generated.h"

class USkeletalMeshComponent;

DECLARE_DELEGATE_OneParam(FOnHit, const TArray<FHitResult>& HitResults);

/**
 *
 */
UCLASS()
class PROJECTM_API UMAbilityTask_SphereTracer : public UAbilityTask
{
	GENERATED_BODY()
/*
* Overrided Functions
*/
public:
	static UMAbilityTask_SphereTracer* CreateSphereTracerTask(
		UGameplayAbility* InOwningAbility,
		const USkeletalMeshComponent* InSkeletalMeshComp,
		float InRadius,
		const FString& InStartSocketName,
		const FString& InEndSocketName,
		const TArray<TEnumAsByte<EObjectTypeQuery>>& InObjectTypes,
		FOnHit&& Callback,
		const TArray<AActor*>& InActorsToIgnore);

protected:
    virtual void TickTask(float DeltaTime) override;
    virtual void Activate() override;
    virtual void OnDestroy(bool bInOwnerFinished) override;

/*
* Member Functions
*/
protected:
	void Init(const USkeletalMeshComponent* InSkeletalMeshComp,
		float InRadius,
		const FString& InStartSocketName,
		const FString& InEndSocketName,
		const TArray<TEnumAsByte<EObjectTypeQuery>>& InObjectTypes,
		FOnHit&& Callback,
		const TArray<AActor*>& InActorsToIgnore);

/*
* Member Variables
*/
protected:
	UPROPERTY()
	const USkeletalMeshComponent* SkeletalMeshComp;

	UPROPERTY()
	float Radius;

	UPROPERTY()
	FString StartSocketName;

	UPROPERTY()
	FString EndSocketName;

	UPROPERTY()
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UPROPERTY()
	TArray<AActor*> ActorsToIgnore;

	FOnHit Delegate_OnHit;
};