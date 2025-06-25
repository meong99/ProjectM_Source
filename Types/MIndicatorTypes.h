// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "MIndicatorTypes.generated.h"

class AMRangeIndicatorActor;
class UDecalComponent;

USTRUCT(BlueprintType)
struct FMIndicatorInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<AMRangeIndicatorActor> IndicatorActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TObjectPtr<UDecalComponent>	IndicatorDecal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float	Angle = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float   Radius = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float   LineThickness = 0.05;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FColor	IndicatorColor = FColor::Red;
};