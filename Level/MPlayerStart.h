// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerStart.h"
#include "MPlayerStart.generated.h"

/**
 *
 */
UCLASS()
class PROJECTM_API AMPlayerStart : public APlayerStart
{
	GENERATED_BODY()
/*
* Overrided Function
*/
public:
	AMPlayerStart(const FObjectInitializer& ObjectInitializer);

/*
* Member Functions
*/
public:

protected:

/*
* Member Variables
*/
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ProjectM")
	FGameplayTag LevelTag;
};
