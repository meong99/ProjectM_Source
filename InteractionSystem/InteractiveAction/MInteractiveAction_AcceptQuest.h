// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MInteractiveAction_OnInteractionBase.h"
#include "Templates/SubclassOf.h"
#include "MInteractiveAction_AcceptQuest.generated.h"

class UInputComponent;
class UMQuestDefinition;
class UMViewportClient;

UCLASS()
class PROJECTM_API UMInteractiveAction_AcceptQuest : public UMInteractiveAction_OnInteractionBase
{
	GENERATED_BODY()

	/*
	* Overrided Functions
	*/
public:
	UMInteractiveAction_AcceptQuest(const FObjectInitializer& ObjectInitializer);

	virtual void InitAction(UMInteractionComponent* InInteractionComponent, AActor* InOwnerActor) override;
	virtual void ActivateAction() override;
	virtual void DeactivateAction() override;

/*
* Member Functions
*/
public:
	void OnClick_Accept();
	void OnClick_Refuse();

protected:
/*
* Member Variables
*/
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (), Category = "Quest")
	int32 QuestRowId;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
	TSubclassOf<UMQuestDefinition> QuestDefinitionClass;
};
