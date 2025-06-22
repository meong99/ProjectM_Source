// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MInteractiveAction_OverlapActionBase.h"
#include "MInteractiveAction_ShowWidget.generated.h"

class UMInteractionComponent;
class UMNameWidgetComponent;

UCLASS()
class PROJECTM_API UMInteractiveAction_ShowWidget : public UMInteractiveAction_OverlapActionBase
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UMInteractiveAction_ShowWidget(const FObjectInitializer& ObjectInitializer);

	virtual void InitAction(UMInteractionComponent* InInteractionComponent, AActor* InOwnerActor) override;
	virtual void ActivateAction() override;
	virtual void DeactivateAction() override;

/*
* Member Functions
*/
public:

protected:
	void EnableNameWidget() const;
	void DisableNameWidget() const;

	UMNameWidgetComponent* GetNameWidgetComponent() const;
/*
* Member Variables
*/
protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<FGameplayTag> WidgetTags;
};
