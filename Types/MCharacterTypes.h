// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MCharacterTypes.generated.h"

UENUM(BlueprintType)
enum class EMCharacterLiftState : uint8
{
	None,
	WaitToSpawn,
	Spawned,
	Alive,
	ReadyToDead,
	Dead,
	ReadyToDestroy,
	Destroied,
};

enum EMCharacterStateFlag : uint64
{
	None_M =					0x0000000000000000,
	BlockMovement =				0x0000000000000001,
	ControlledFromNavigation =	0x0000000000000002,
	BlockAll =					0x0000000000000005,
};
