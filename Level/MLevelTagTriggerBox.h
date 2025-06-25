// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "GameplayTagContainer.h"
#include "MLevelTagTriggerBox.generated.h"

/**
 *
 */
UCLASS()
class PROJECTM_API AMLevelTagTriggerBox : public ATriggerBox
{
	GENERATED_BODY()
/*
* Overrided Function
*/
public:
	AMLevelTagTriggerBox();
	virtual void PostInitializeComponents() override;

/*
* Member Functions
*/
public:

protected:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
/*
* Member Variables
*/
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ProjectM")
	FGameplayTag GrantedLevelTag;
};
