#include "MAbilityTask_SphereTracer.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Util/MGameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"

UMAbilityTask_SphereTracer* UMAbilityTask_SphereTracer::CreateSphereTracerTask(
	UGameplayAbility* InOwningAbility, const USkeletalMeshComponent* InSkeletalMeshComp, float InRadius,
	const FString& InStartSocketName, const FString& InEndSocketName, const TArray<TEnumAsByte<EObjectTypeQuery>>& InObjectTypes, 
	FOnHit&& Callback, const TArray<AActor*>& InActorsToIgnore)
{
	UMAbilityTask_SphereTracer* MyObj = NewAbilityTask<UMAbilityTask_SphereTracer>(InOwningAbility);
	MyObj->Init(InSkeletalMeshComp, InRadius, InStartSocketName, InEndSocketName, InObjectTypes, MoveTemp(Callback), InActorsToIgnore);
	return MyObj;
}

void UMAbilityTask_SphereTracer::TickTask(float DeltaTime)
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
	UKismetSystemLibrary::SphereTraceMultiForObjects(this, Start, End, Radius, ObjectTypes, false, ActorsToIgnore, DebugType, OutHits, true);
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

void UMAbilityTask_SphereTracer::Activate()
{
	Super::Activate();

	bTickingTask = true;
}

void UMAbilityTask_SphereTracer::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}

void UMAbilityTask_SphereTracer::Init(
	const USkeletalMeshComponent* InSkeletalMeshComp, float InRadius, const FString& InStartSocketName, 
	const FString& InEndSocketName, const TArray<TEnumAsByte<EObjectTypeQuery>>& InObjectTypes, 
	FOnHit&& Callback, const TArray<AActor*>& InActorsToIgnore)
{
	SkeletalMeshComp = InSkeletalMeshComp;
	Radius = InRadius;
	StartSocketName = InStartSocketName;
	EndSocketName = InEndSocketName;
	ObjectTypes = InObjectTypes;
	Delegate_OnHit = MoveTemp(Callback);
	ActorsToIgnore = InActorsToIgnore;
}
