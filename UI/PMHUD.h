// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "PMHUD.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTM_API APMHUD : public AHUD
{
	GENERATED_BODY()
public:
	APMHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	 */
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
