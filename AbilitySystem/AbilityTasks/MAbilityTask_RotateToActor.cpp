#include "MAbilityTask_RotateToActor.h"
#include "Engine/World.h"

UMAbilityTask_RotateToActor* UMAbilityTask_RotateToActor::CreateRotateToActorTask(UGameplayAbility* InOwningAbility, const AActor* InTargetActor, float AngleTolerance)
{
	UMAbilityTask_RotateToActor* MyObj = NewAbilityTask<UMAbilityTask_RotateToActor>(InOwningAbility);
	MyObj->OwningAbility = InOwningAbility;
	MyObj->TargetActor = InTargetActor;
	MyObj->ToleranceDegrees = AngleTolerance;
	MyObj->bDone = false;
	return MyObj;
}

void UMAbilityTask_RotateToActor::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (bDone || !TargetActor || !GetAvatarActor())
	{
		EndTask();
	}

	CheckAlignmentAndRotate();
}

void UMAbilityTask_RotateToActor::Activate()
{
	Super::Activate();
	if (!TargetActor || !GetAvatarActor())
	{
		EndTask();
		return;
	}

	bTickingTask = true;
}

void UMAbilityTask_RotateToActor::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}

void UMAbilityTask_RotateToActor::CheckAlignmentAndRotate()
{
	AActor* Owner = GetAvatarActor();
	if (!Owner || !TargetActor) return;

	FVector ToTarget = TargetActor->GetActorLocation() - Owner->GetActorLocation();
	ToTarget.Z = 0;
	ToTarget.Normalize();

	FVector Forward = Owner->GetActorForwardVector();
	Forward.Z = 0;
	Forward.Normalize();

	float Angle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(Forward, ToTarget)));

	if (Angle > ToleranceDegrees)
	{
		FRotator TargetRot = ToTarget.Rotation();
		FRotator Current = Owner->GetActorRotation();
		FRotator NewRot = FMath::RInterpTo(Current, TargetRot, GetWorld()->GetDeltaSeconds(), 10.f); // 부드럽게
		Owner->SetActorRotation(NewRot);
	}
	else
	{
		bDone = true;
		EndTask();
	}
}

