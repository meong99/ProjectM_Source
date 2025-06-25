// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/PMGameplayAbility_RangedWeapon.h"
#include "PMRangedWeaponInstance.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "CollisionQueryParams.h"
#include "Physics/PMkCollisionChannels.h"
#include "Engine/World.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/PMGameplayAbilityTargetData_SingleTarget.h"
#include "DrawDebugHelpers.h"
#include "Util/MGameplayStatics.h"

UPMGameplayAbility_RangedWeapon::UPMGameplayAbility_RangedWeapon()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void UPMGameplayAbility_RangedWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// Bind target data callback
	UAbilitySystemComponent* MyAbilityComponent = CurrentActorInfo->AbilitySystemComponent.Get();
	check(MyAbilityComponent);

	OnTargetDataReadyCallbackDelegateHandle = MyAbilityComponent->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).AddUObject(this, &ThisClass::OnTargetDataReadyCallback);

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UPMGameplayAbility_RangedWeapon::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsEndAbilityValid(Handle, ActorInfo))
	{
		if (ScopeLockCount > 0)
		{
			WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &ThisClass::EndAbility, Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled));
			return;
		}

		UAbilitySystemComponent* MyAbilityComponent = CurrentActorInfo->AbilitySystemComponent.Get();
		check(MyAbilityComponent);

		// When ability ends, consume target data and remove delegate
		MyAbilityComponent->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).Remove(OnTargetDataReadyCallbackDelegateHandle);
		MyAbilityComponent->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());

		Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	}
}

void UPMGameplayAbility_RangedWeapon::StartRangedWeaponTargeting()
{
	check(CurrentActorInfo);

	AActor* AvatarActor = CurrentActorInfo->AvatarActor.Get();
	check(AvatarActor);

	UAbilitySystemComponent* MyAbilityComponent = CurrentActorInfo->AbilitySystemComponent.Get();
	check(MyAbilityComponent);

	// 총알 궤적 계산 및 Hit 정보 계산
	TArray<FHitResult> FoundHits;
	PerformLocalTargeting(FoundHits);

	// 데디, 클라간의 공유데이터.
	FGameplayAbilityTargetDataHandle TargetData;
	TargetData.UniqueId = 0;

	if (FoundHits.Num() > 0)
	{
		// 총알에 대한 정보
		const int32 CartridgeID = FMath::Rand();

		// 피격 대상에게 보낼 정보를 담음
		for (const FHitResult& FoundHit : FoundHits)
		{
			FPMGameplayAbilityTargetData_SingleTargetHit* NewTargetData = new FPMGameplayAbilityTargetData_SingleTargetHit();
			NewTargetData->HitResult = FoundHit;
			NewTargetData->CartridgeID = CartridgeID;
			TargetData.Add(NewTargetData);
		}
	}

	// 피격 대상에게 정보 전송
	OnTargetDataReadyCallback(TargetData, FGameplayTag());
}

void UPMGameplayAbility_RangedWeapon::PerformLocalTargeting(TArray<FHitResult>& OutHits)
{
	APawn* const AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());

	UPMRangedWeaponInstance* WeaponData = GetWeaponInstance();
	if (AvatarPawn && WeaponData)
	{
		FRangedWeaponFiringInput InputData;
		InputData.WeaponData = WeaponData;
		InputData.bCanPlayBulletFX = true;

		const FTransform TargetTransform = GetTargetingTransform(AvatarPawn, EPMAbilityTargetingSource::CameraTowardsFocus);
		InputData.AimDir = TargetTransform.GetUnitAxis(EAxis::X);
		InputData.StartTrace = TargetTransform.GetTranslation();
		InputData.EndAim = InputData.StartTrace + InputData.AimDir * WeaponData->MaxDamageRange;

		if (UMGameplayStatics::bShowDebug_Console)
		{
			static float DebugThickness = 2.0f;
			DrawDebugLine(GetWorld(), InputData.StartTrace, InputData.StartTrace + (InputData.AimDir * 100.0f), FColor::Yellow, false, 10.0f, 0, DebugThickness);
		}

		TraceBulletsInCartridge(InputData, OutHits);
	}
}

void UPMGameplayAbility_RangedWeapon::TraceBulletsInCartridge(const FRangedWeaponFiringInput& InputData, TArray<FHitResult>& OutHits)
{
	UPMRangedWeaponInstance* WeaponData = InputData.WeaponData;
	check(WeaponData);

	// MaxDamageRange를 고려한 EndTrace
	const FVector BulletDir = InputData.AimDir;
	const FVector EndTrace = InputData.StartTrace + (BulletDir * WeaponData->MaxDamageRange);

	// HitLocation의 초기값으로 EndTrace설정
	FVector HitLocation = EndTrace;

	// 총알을 하나 Trace한다.
	// 참고로 라이라의 경우 샷건과 같은 Cartridge가 여러개인 경우 For-Loop를 사용해 여러개의 Bullet을 Tracing한다
	TArray<FHitResult> AllImpacts;
	FHitResult Impact = DoSingleBulletTrace(InputData.StartTrace, EndTrace, WeaponData->BulletTraceWeaponRadius, /**bIsSimulated=*/ false, /**out*/ AllImpacts);

	const AActor* HitActor = Impact.GetActor();
	if (HitActor)
	{
		if (AllImpacts.Num() > 0)
		{
			OutHits.Append(AllImpacts);
		}

		HitLocation = Impact.ImpactPoint;
	}

	// OutHits가 적어도 하나가 존재하도록 EndTrace를 활용해 OutHits에 추가
	if (OutHits.Num() == 0)
	{
		if (!Impact.bBlockingHit)
		{
			Impact.Location = EndTrace;
			Impact.ImpactPoint = EndTrace;
		}

		OutHits.Add(Impact);
	}
}

void UPMGameplayAbility_RangedWeapon::AddAdditionalTraceIgnoreActors(FCollisionQueryParams& TraceParams) const
{
	if (AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		TArray<AActor*> AttachedActors;

		// GetAttachedActors를 한 번 보자.
		// 해당 함수는 Recursively하게 모든 Acotrs를 추출한다.
		// 근데 왜 앞서 FindFirstPawnHitsReulst 이건 왜 한 단계만 할까?
		Avatar->GetAttachedActors(AttachedActors);

		TraceParams.AddIgnoredActors(AttachedActors);
	}
}

void UPMGameplayAbility_RangedWeapon::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag)
{
	UAbilitySystemComponent* MyAbilityComponent = CurrentActorInfo->AbilitySystemComponent.Get();
	check(MyAbilityComponent);

	if (const FGameplayAbilitySpec* AbilitySpec = MyAbilityComponent->FindAbilitySpecFromHandle(CurrentSpecHandle))
	{
		// 현재 Stack에서 InData에서 지금 Local로 Ownership을 가져온다
		FGameplayAbilityTargetDataHandle LocalTargetDataHandle(MoveTemp(const_cast<FGameplayAbilityTargetDataHandle&>(InData)));

		const bool bShouldNotifyServer = CurrentActorInfo->IsLocallyControlled() && !CurrentActorInfo->IsNetAuthority();
		if (bShouldNotifyServer)
		{
			MyAbilityComponent->CallServerSetReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey(), LocalTargetDataHandle, ApplicationTag, MyAbilityComponent->ScopedPredictionKey);
		}

		const bool bIsTargetDataValid = true;

		bool bProjectileWeapon = false;

		// CommitAbility 호출로 GE를 처리한다.
		if (CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
		{
			// 여기서 CueNotift를 처리한다.
			OnRangeWeaponTargetDataReady(LocalTargetDataHandle);
		}
		else
		{
			K2_EndAbility();
		}
	}

	MyAbilityComponent->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
}

FTransform UPMGameplayAbility_RangedWeapon::GetTargetingTransform(APawn* SourcePawn, EPMAbilityTargetingSource Source)
{
	check(SourcePawn);
	check(Source == EPMAbilityTargetingSource::CameraTowardsFocus);

	AController* Controller = SourcePawn->Controller;
	if (Controller == nullptr)
	{
		return FTransform();
	}

	double FocalDistance = 1024.0f;
	FVector FocalLoc;
	FVector CamLoc;
	FRotator CamRot;

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	check(PlayerController);
	PlayerController->GetPlayerViewPoint(CamLoc, CamRot);

	FVector AimDir = CamRot.Vector().GetSafeNormal();
	FocalLoc = CamLoc + (AimDir * FocalDistance);

	const FVector WeaponLoc = GetWeaponTargetingSourceLocation();
	FVector FinalCamLoc = FocalLoc + (((WeaponLoc - FocalLoc) | AimDir) * AimDir);

	if (UMGameplayStatics::bShowDebug_Console)
	{
		DrawDebugPoint(GetWorld(), WeaponLoc, 10.0f, FColor::Red, false, 60.0f);
		DrawDebugPoint(GetWorld(), CamLoc, 10.0f, FColor::Yellow, false, 60.0f);
		DrawDebugPoint(GetWorld(), FinalCamLoc, 10.0f, FColor::Magenta, false, 60.0f);

		DrawDebugLine(GetWorld(), FocalLoc, WeaponLoc, FColor::Yellow, false, 60.0f, 0, 2.0f);
		DrawDebugLine(GetWorld(), CamLoc, FocalLoc, FColor::Blue, false, 60.0f, 0, 2.0f);
		DrawDebugLine(GetWorld(), WeaponLoc, FinalCamLoc, FColor::Red, false, 60.0f, 0, 2.0f);
	}

	return FTransform(CamRot, FinalCamLoc);
}

FVector UPMGameplayAbility_RangedWeapon::GetWeaponTargetingSourceLocation() const
{
	// 미구현인 상태. 현재 Pawn의 Location을 가져온다.
	APawn* const AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	check(AvatarPawn);

	const FVector SourceLoc = AvatarPawn->GetActorLocation();
	return SourceLoc;
}

int32 FindFirstPawnHitResult(const TArray<FHitResult>& HitResults)
{
	for (int32 Idx = 0; Idx < HitResults.Num(); ++Idx)
	{
		const FHitResult& CurHitResult = HitResults[Idx];
		if (CurHitResult.HitObjectHandle.DoesRepresentClass(APawn::StaticClass()))
		{
			return Idx;
		}
		else
		{
			AActor* HitActor = CurHitResult.HitObjectHandle.FetchActor();

			// 한 단계 AttachParent에 Actor가 Pawn이라면?
			// 보통 복수개 단계로 AttachParent를 하지 않으므로, AttachParent 대상이 APawn이라고 생각할 수 있겠다.
			if ((HitActor != nullptr) && (HitActor->GetAttachParentActor() != nullptr) && (Cast<APawn>(HitActor->GetAttachParentActor()) != nullptr))
			{
				return Idx;
			}
		}
	}
	return INDEX_NONE;
}

FHitResult UPMGameplayAbility_RangedWeapon::DoSingleBulletTrace(const FVector& StartTrace, const FVector& EndTrace, float SweepRadius, bool bIsSimulated, TArray<FHitResult>& OutHits) const
{
	FHitResult Impact;

	// 우선 SweepRadius 없이 한 번 Trace한다.(무거워서)
	// FindFirstPawnHitResult()를 여러번 진행하는걸 막기 위해, OutHits를 확인해서 APawn 충돌 정보 있으면 더 이상 Tracing하지 않는다.
	if (FindFirstPawnHitResult(OutHits) == INDEX_NONE)
	{
		Impact = WeaponTrace(StartTrace, EndTrace, /*SweepRadius=*/0.0f, bIsSimulated, /*out*/ OutHits);
	}

	if (FindFirstPawnHitResult(OutHits) == INDEX_NONE)
	{
		// 만약 SweepRadius가 0보다 크면 0.0일 때 대비 충돌 가능성이 커지므로 한 번 더 진행
		if (SweepRadius > 0.0f)
		{
			TArray<FHitResult> SweepHits;
			Impact = WeaponTrace(StartTrace, EndTrace, SweepRadius, bIsSimulated, SweepHits);

			// Sphere Trace로 진행한 결과인 SweepHits를 검색하여, Pawn이 있는지 검색
			const int32 FirstPawnIdx = FindFirstPawnHitResult(SweepHits);
			if (SweepHits.IsValidIndex(FirstPawnIdx))
			{
				// 만약 있다면, FirstPawnIdx까지 순회하며 체크
				bool bUseSweepHits = true;
				for (int32 Idx = 0; Idx < FirstPawnIdx; ++Idx)
				{
					const FHitResult& CurHitResult = SweepHits[Idx];

					auto Pred = [&CurHitResult](const FHitResult& Other)
						{
							return Other.HitObjectHandle == CurHitResult.HitObjectHandle;
						};

					// OutHits에 있다면 SweeoHits를 OutHits로 업데이트 하지 않는다.
					if (CurHitResult.bBlockingHit && OutHits.ContainsByPredicate(Pred))
					{
						bUseSweepHits = false;
						break;
					}
				}

				if (bUseSweepHits)
				{
					OutHits = SweepHits;
				}
			}
		}
	}

	return Impact;
}

FHitResult UPMGameplayAbility_RangedWeapon::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace, float SweepRadius, bool bIsSimulated, TArray<FHitResult>& OutHitResults) const
{
	TArray<FHitResult> HitResults;

	// 자신은 충돌검사 무시
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), /*bTraceComplex*/true, /*IgnoreActor=*/GetAvatarActorFromActorInfo());
	TraceParams.bReturnPhysicalMaterial = true;

	// Avatar에 부착된 Actors를 찾아 Ignore에 추가
	AddAdditionalTraceIgnoreActors(TraceParams);

	const ECollisionChannel TraceChannel = DetermineTraceChannel(TraceParams, bIsSimulated);
	if (SweepRadius > 0.0f)
	{
		GetWorld()->SweepMultiByChannel(HitResults, StartTrace, EndTrace, FQuat::Identity, TraceChannel, FCollisionShape::MakeSphere(SweepRadius), TraceParams);
	}
	else
	{
		GetWorld()->LineTraceMultiByChannel(HitResults, StartTrace, EndTrace, TraceChannel, TraceParams);
	}

	FHitResult Hit(ForceInit);
	if (HitResults.Num() > 0)
	{
		for (FHitResult& CurHitResult : HitResults)
		{
			auto Pred = [&CurHitResult](const FHitResult& Other)
				{
					return Other.HitObjectHandle == CurHitResult.HitObjectHandle;
				};

			if (!OutHitResults.ContainsByPredicate(Pred))
			{
				OutHitResults.Add(CurHitResult);
			}
		}

		Hit = OutHitResults.Last();
	}
	else
	{
		Hit.TraceStart = StartTrace;
		Hit.TraceEnd = EndTrace;
	}

	return Hit;
}

ECollisionChannel UPMGameplayAbility_RangedWeapon::DetermineTraceChannel(FCollisionQueryParams& TraceParams, bool bIsSimulated) const
{
	return PM_TraceChannel_Weapon;
}

UPMRangedWeaponInstance* UPMGameplayAbility_RangedWeapon::GetWeaponInstance()
{
	return Cast<UPMRangedWeaponInstance>(GetAssociatedEquipment());
}
