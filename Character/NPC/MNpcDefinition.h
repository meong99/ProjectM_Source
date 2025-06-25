// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Definitions/MDefinitionBase.h"
#include "GameplayTagContainer.h"
#include "MNpcDefinition.generated.h"

class UMNameWidgetComponent;
class UMInteractiveAction_OnInteractionBase;
class UMInteractiveAction_OverlapActionBase;
class AMNpcBase;

/**
 *
 */
UCLASS(Abstract, Blueprintable)
class PROJECTM_API UMNpcDefinition : public UMDefinitionBase
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UMNpcDefinition();

/*
* Member Functions
*/
public:

/*
* Member Variables
*/
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectM")
	TSubclassOf<AMNpcBase> NpcClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectM")
	FName NpcName;

	// NPC가 있는 WorldTag
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectM")
	FGameplayTag WorldTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectM")
	FGameplayTag SearchTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "ProjectM")
	TArray<TObjectPtr<UMInteractiveAction_OverlapActionBase>> Action_OnBeginOverlap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "ProjectM")
	TArray<TObjectPtr<UMInteractiveAction_OnInteractionBase>> Action_OnInteract;
};
