#include "MAbilityTask_CapsuleTracer.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Util/MGameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"

UMAbilityTask_CapsuleTracer* UMAbilityTask_CapsuleTracer::CreateCapsuleTracerTask(UGameplayAbility* InOwningAbility, const USkeletalMeshComponent* InSkeletalMeshComp, float InRadius,
	float InHalfHeight, const FString& InStartSocketName, const FString& InEndSocketName,
	const TArray<TEnumAsByte<EObjectTypeQuery>>& InObjectTypes, FOnHit&& Callback, const TArray<AActor*>& InActorsToIgnore)
{
	UMAbilityTask_CapsuleTracer* MyObj = NewAbilityTask<UMAbilityTask_CapsuleTracer>(InOwningAbility);
	MyObj->Init(InSkeletalMeshComp, InRadius, InHalfHeight, InStartSocketName, InEndSocketName, InObjectTypes, MoveTemp(Callback), InActorsToIgnore);
	return MyObj;
}

void UMAbilityTask_CapsuleTracer::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (!SkeletalMeshComp)
	{
		return;
	}

	FVector Start = SkeletalMeshComp->GetSocketLocation(*StartSocketName);
	FVector End = SkeletalMeshComp->GetSocketLocation(*EndSocketName);;

	EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;
	if (UMGameplayStatics::bShowDebug_Console)
	{
		DebugType = EDrawDebugTrace::ForDuration;
	}
	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::CapsuleTraceMultiForObjects(this, Start, End, Radius, HalfHeight, ObjectTypes, false, ActorsToIgnore, DebugType, OutHits, true);
	if (OutHits.Num())
	{
		for (const FHitResult& HitResult : OutHits)
		{
			if (HitResult.GetActor())
			{
				ActorsToIgnore.Add(HitResult.GetActor());
			}
		}
		Delegate_OnHit.ExecuteIfBound(OutHits);
	}
}

void UMAbilityTask_CapsuleTracer::Activate()
{
	Super::Activate();

	bTickingTask = true;
}

void UMAbilityTask_CapsuleTracer::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}

void UMAbilityTask_CapsuleTracer::Init(const USkeletalMeshComponent* InSkeletalMeshComp, float InRadius, float InHalfHeight,
	const FString& InStartSocketName, const FString& InEndSocketName,
	const TArray<TEnumAsByte<EObjectTypeQuery>>& InObjectTypes, FOnHit&& Callback, const TArray<AActor*>& InActorsToIgnore)
{
	SkeletalMeshComp = InSkeletalMeshComp;
	Radius = InRadius;
	HalfHeight = InHalfHeight;
	StartSocketName = InStartSocketName;
	EndSocketName = InEndSocketName;
	ObjectTypes = InObjectTypes;
	Delegate_OnHit = MoveTemp(Callback);
	ActorsToIgnore = InActorsToIgnore;
}
