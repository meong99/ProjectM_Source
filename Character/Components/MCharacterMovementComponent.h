// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MCharacterMovementComponent.generated.h"

class AActor;

/**
 * 
 */
UCLASS()
class PROJECTM_API UMCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
/*
* Overrided Function
*/
public:
	virtual void InitializeComponent() override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void AddInputVector(FVector WorldVector, bool bForce = false) override;

/*
* Member Functions
*/
public:

/*
* Member Variables
*/
protected:
	TArray<AActor*> VoxelWorlds;
};
