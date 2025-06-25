#include "MNavigationComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/HitResult.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Util/MGameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameModes/PMGameStateBase.h"
#include "Character/MPlayerCharacterBase.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

UMNavigationComponent::UMNavigationComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UMNavigationComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMNavigationComponent, MovementInput);
}

void UMNavigationComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (HasAuthority())
	{
		if (Path && OwnerCharacter)
		{
			if (!OwnerCharacter->IsOnCharacterStateFlags(EMCharacterStateFlag::BlockMovement) &&
				OwnerCharacter->IsOnCharacterStateFlags(EMCharacterStateFlag::ControlledFromNavigation))
			{
				if (0 <= CurrentPathIndex && CurrentPathIndex < Path->PathPoints.Num() - 1)
				{
					int32 NextPathIndex = CurrentPathIndex + 1;
					FVector OwnerLocation = OwnerCharacter->GetActorLocation();
					FVector NextPointLocation = Path->PathPoints[NextPathIndex];
					float NextDist = FVector::Dist(OwnerLocation, NextPointLocation);

					FVector CurrentPointLocation = Path->PathPoints[CurrentPathIndex];
					float CurrentDist = FVector::Dist(OwnerLocation, CurrentPointLocation);

					if (NextPathIndex == Path->PathPoints.Num() - 1 && NextDist <= GoalThreshold)
					{
						CurrentPathIndex++;
					}
					else if (NextDist > DistanceThreshold)
					{
						SetMovementInput((NextPointLocation - OwnerLocation).GetSafeNormal2D());
					}
					else
					{
						CurrentPathIndex++;
					}
				}
				else
				{
					StopAndCheckDest();
				}
			}
			else
			{
				SetMovementInput(FVector::ZeroVector);
			}
		}
		else
		{
			// 무조건 있어야하는 뭔가가 없음!
			ensure(false);
			Server_DeactivateNavigation();
		}
	}

	if (GetNetMode() != ENetMode::NM_DedicatedServer && OwnerCharacter &&
		!OwnerCharacter->IsOnCharacterStateFlags(EMCharacterStateFlag::BlockMovement) &&
		OwnerCharacter->IsOnCharacterStateFlags(EMCharacterStateFlag::ControlledFromNavigation))
	{
		OwnerCharacter->AddMovementInput(MovementInput);
	}
}

void UMNavigationComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(false);
	OwnerCharacter = Cast<AMPlayerCharacterBase>(GetOwner());
}

void UMNavigationComponent::Server_ActivateNavigation_Implementation(const FGameplayTag& SearchTag, const FGameplayTag& OriginLevelTag)
{
	if (!OwnerCharacter)
	{
		ensure(false);
		return;
	}

	DestTags.Empty();
	OwnerCharacter->Server_AddCharacterStateFlag(EMCharacterStateFlag::ControlledFromNavigation);

	FGameplayTag DestLevelTag = SearchTag.RequestDirectParent();

	if (OwnerCharacter)
	{
		if (OwnerCharacter->GetCurrentLevelTag() == DestLevelTag)
		{
			DestTags.Enqueue(SearchTag);
		}
		else
		{
			DestTags.Enqueue(DestLevelTag);
			DestTags.Enqueue(SearchTag);
		}
	}

	GeneratePathData();
	SetComponentTickEnabled(true);
}

void UMNavigationComponent::Server_DeactivateNavigation_Implementation()
{
	SetComponentTickEnabled(false);
	SetMovementInput(FVector::ZeroVector);
	OwnerCharacter->Server_RemoveCharacterStateFlag(EMCharacterStateFlag::ControlledFromNavigation);
}

void UMNavigationComponent::RequestOngoingNavigation()
{
	GeneratePathData();
	SetComponentTickEnabled(true);
}

void UMNavigationComponent::OnRep_MovementInput()
{
	if (MovementInput.IsNearlyZero())
	{
		SetComponentTickEnabled(false);
	}
	else
	{
		SetComponentTickEnabled(true);
	}
}

void UMNavigationComponent::SetMovementInput(const FVector& NewMovementInput)
{
	if (MovementInput != NewMovementInput)
	{
		MovementInput = NewMovementInput;
	}
}

void UMNavigationComponent::GeneratePathData()
{
	CurrentPathIndex = 0;
	if (DestTags.IsEmpty())
	{
		Server_DeactivateNavigation();
		return;
	}

	FVector Dest = PopDestLocation();

	if (Dest.IsZero())
	{
		MCHAE_WARNING("Target vector is zero!");
		return;
	}

	FVector PathStart = FVector::ZeroVector;
	if (OwnerCharacter->GetCharacterMovement()->IsFalling())
	{
		bool bHit = false;
		PathStart = GetFloorLocation(OwnerCharacter, bHit);
	}
	if (PathStart == FVector::ZeroVector)
	{
		PathStart = OwnerCharacter->GetActorLocation();
	}

	Path = UNavigationSystemV1::FindPathToLocationSynchronously(this, PathStart, Dest);
	if (Path && Path->PathPoints.Num() >= 2)
	{
		FVector NextPointLocation = Path->PathPoints[CurrentPathIndex + 1];
		NextPointLocation = { NextPointLocation.X, NextPointLocation.Y, 0.0f };
		float Dist = FVector::Dist(PathStart, NextPointLocation);
	}

#if WITH_EDITOR
	ShowPathPointDebugLine();
#endif
}

void UMNavigationComponent::StopAndCheckDest()
{
	SetMovementInput(FVector::ZeroVector);
	SetComponentTickEnabled(false);
	if (DestTags.IsEmpty())
	{
		Server_DeactivateNavigation();
	}
}

void UMNavigationComponent::ShowPathPointDebugLine()
{
	if (UMGameplayStatics::bShowDebug_Console && Path && Path->PathPoints.Num() > 0)
	{
		TArray<FHitResult> Result;
		for (int32 i = 0; i < Path->PathPoints.Num(); i++)
		{
			UKismetSystemLibrary::LineTraceMulti(this, Path->PathPoints[i], Path->PathPoints[i] + (FVector::UpVector * 100),
				ETraceTypeQuery::TraceTypeQuery1, false, {}, EDrawDebugTrace::ForDuration, Result, true, FLinearColor::Red, FLinearColor::Green, 10);
		}
	}
}

FVector UMNavigationComponent::PopDestLocation()
{
	FGameplayTag DestTag;
	DestTags.Dequeue(DestTag);

	APMGameStateBase* GameState = Cast<APMGameStateBase>(GetWorld()->GetGameState());
	AActor* Target = GameState->TagMappedActor.FindRef(DestTag);
	bool bHit = false;
	FVector Loc = GetFloorLocation(Target, bHit);
	if (bHit)
	{
		return Loc;
	}

	return FVector::ZeroVector;
}

FVector UMNavigationComponent::GetFloorLocation(AActor* TargetActor, bool& bHit) const
{
	if (!TargetActor)
	{
		bHit = false;
		return FVector::ZeroVector;
	}
	TArray<AActor*> ActorsToIgnore = { TargetActor };
	EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;
	if (UMGameplayStatics::bShowDebug_Console)
	{
		DebugType = EDrawDebugTrace::ForDuration;
	}
	FHitResult OutHit;
	UKismetSystemLibrary::LineTraceSingle(this, TargetActor->GetActorLocation(),
										  TargetActor->GetActorLocation() + FVector::DownVector * 1000, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_WorldStatic),
										  false, ActorsToIgnore, DebugType, OutHit, true);
	bHit = OutHit.bBlockingHit;
	return OutHit.ImpactPoint;
}
