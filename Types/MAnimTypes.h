// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MAnimTypes.generated.h"

class UAnimMontage;

USTRUCT(BlueprintType)
struct FMMontageInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Rate = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName StartSection = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float StartTimeSeconds = 0.f;
};