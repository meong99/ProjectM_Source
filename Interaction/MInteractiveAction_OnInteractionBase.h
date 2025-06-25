// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MInteractiveAction_Base.h"
#include "MInteractiveAction_OnInteractionBase.generated.h"

UCLASS(Abstract)
class PROJECTM_API UMInteractiveAction_OnInteractionBase : public UMInteractiveAction_Base
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UMInteractiveAction_OnInteractionBase(const FObjectInitializer& ObjectInitializer);
	virtual void ActivateAction() override;
	virtual void DeactivateAction() override;

		/*
* Member Functions
*/
public:
	const FText&	GetActionName() const { return ActionName; }
	bool			ShouldActivate() const { return bShouldActivate; }
/*
* Member Variables
*/
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Action")
	FText ActionName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Action")
	bool bShouldActivate = true;
};
