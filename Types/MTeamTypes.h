// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EMGenericTeamId : uint8
{
	Player,
	Monster,
	Npc,
	None,
};