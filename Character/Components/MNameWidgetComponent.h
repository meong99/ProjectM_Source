// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "MNameWidgetComponent.generated.h"

class AMNpcBase;
class APawn;
class ACharacter;
class UMNameWidget;
class APlayerCameraManager;

/**
 * 
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class PROJECTM_API UMNameWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UMNameWidgetComponent();

	virtual void InitWidget() override;

/*
* Member Functions
*/
public:
	UFUNCTION(BlueprintCallable)
	void InitNameWidget();
	void EnableNameWidget();
	void DisableNameWidget();

protected:
	void AdjustNameWidgetCompLocation();

/*
* Member Variables
*/
protected:
	TWeakObjectPtr<ACharacter> WeakOwnerCharacter;
	UMNameWidget* NameWidget;

	UPROPERTY(EditDefaultsOnly)
	float MinDist = 0.f;
	UPROPERTY(EditDefaultsOnly)
	float MaxDist = 1000.f;
};
