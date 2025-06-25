// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonUserWidget.h"
#include "UObject/ObjectPtr.h"
#include "UObject/UObjectGlobals.h"
#include "PMWeaponUserInterface.generated.h"

class UPMWeaponInstance;

/**
 * Deprecated
 */
UCLASS()
class PROJECTM_API UPMWeaponUserInterface : public UCommonUserWidget
{
	GENERATED_BODY()
public:
	UPMWeaponUserInterface(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintImplementableEvent)
	void OnWeaponChanged(UPMWeaponInstance* OldWeapon, UPMWeaponInstance* NewWeapon);

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(Transient)
	TObjectPtr<UPMWeaponInstance> CurrentInstance;
};
