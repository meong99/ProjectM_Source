#include "Character/Components/MCharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/MCharacterBase.h"

void UMCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UMCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
}

void UMCharacterMovementComponent::AddInputVector(FVector WorldVector, bool bForce /*= false*/)
{
	Super::AddInputVector(WorldVector, bForce);
}
