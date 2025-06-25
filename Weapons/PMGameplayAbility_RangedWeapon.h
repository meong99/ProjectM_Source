// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/PMGameplayAbility_FromEquipment.h"
#include "PMGameplayAbility_RangedWeapon.generated.h"

class UPMRangedWeaponInstance;

// 어디를 기준으로 Target을 설정할지 옵션
UENUM(BlueprintType)
enum class EPMAbilityTargetingSource : uint8
{
	// 카메라 기준으로 진행
	CameraTowardsFocus,
};

/**
 * 
 */
UCLASS()
class PROJECTM_API UPMGameplayAbility_RangedWeapon : public UPMGameplayAbility_FromEquipment
{
	GENERATED_BODY()
	
/*
* Overrided Functions
*/
public:
	UPMGameplayAbility_RangedWeapon();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	/*
* Member Functions
*/
public:
	struct FRangedWeaponFiringInput
	{
		FVector StartTrace;
		FVector EndAim;
		FVector AimDir;
		UPMRangedWeaponInstance* WeaponData = nullptr;

		// 총알을 쐈을 때 탄피 보여줄건지
		bool bCanPlayBulletFX = false;

		FRangedWeaponFiringInput()
			: StartTrace(ForceInitToZero)
			, EndAim(ForceInitToZero)
			, AimDir(ForceInitToZero)
		{}
	};

protected:
	UFUNCTION(BlueprintCallable)
	void StartRangedWeaponTargeting();

	void PerformLocalTargeting(TArray<FHitResult>& OutHits);
	void TraceBulletsInCartridge(const FRangedWeaponFiringInput& InputData, TArray<FHitResult>& OutHits);
	void AddAdditionalTraceIgnoreActors(FCollisionQueryParams& TraceParams) const;
	void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag);

	FTransform			GetTargetingTransform(APawn* SourcePawn, EPMAbilityTargetingSource Source);
	FVector				GetWeaponTargetingSourceLocation() const;
	FHitResult			DoSingleBulletTrace(const FVector& StartTrace, const FVector& EndTrace, float SweepRadius, bool bIsSimulated, TArray<FHitResult>& OutHits) const;
	FHitResult			WeaponTrace(const FVector& StartTrace, const FVector& EndTrace, float SweepRadius, bool bIsSimulated, TArray<FHitResult>& OutHitResults) const;
	ECollisionChannel	DetermineTraceChannel(FCollisionQueryParams& TraceParams, bool bIsSimulated) const;

	UFUNCTION(BlueprintImplementableEvent)
	void OnRangeWeaponTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetData);

	UPMRangedWeaponInstance* GetWeaponInstance();

	/*
* Member Variables
*/
public:

protected:
	FDelegateHandle OnTargetDataReadyCallbackDelegateHandle;
};
