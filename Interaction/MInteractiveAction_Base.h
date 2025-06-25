// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MInteractiveAction_Base.generated.h"

class UMInteractionComponent;
class APMPlayerControllerBase;
class AActor;

UCLASS(Abstract, Blueprintable, BlueprintType, DefaultToInstanced, EditInlineNew)
class PROJECTM_API UMInteractiveAction_Base : public UObject
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UMInteractiveAction_Base(const FObjectInitializer& ObjectInitializer);

/*
* Member Functions
*/
public:
	virtual void InitAction(UMInteractionComponent* InInteractionComponent, AActor* InOwnerActor);
	UFUNCTION(BlueprintCallable)
	virtual void ActivateAction();
	UFUNCTION(BlueprintCallable)
	virtual void DeactivateAction();

	UFUNCTION(BlueprintImplementableEvent)
	void K2_InitAction(UMInteractionComponent* InInteractionComponent, AActor* InOwnerActor, APMPlayerControllerBase* Controller);
	UFUNCTION(BlueprintImplementableEvent)
	void K2_ActivateAction();
	UFUNCTION(BlueprintImplementableEvent)
	void K2_DeactivateAction();

/*
* Member Variables
*/
protected:
	UPROPERTY(BlueprintReadOnly, Category = "ProjectM")
	AActor* OwnerActor;

	UPROPERTY(BlueprintReadOnly, Category = "ProjectM")
	TObjectPtr<UMInteractionComponent> InteractionComponent;
};
