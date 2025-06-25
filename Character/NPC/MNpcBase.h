// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/MCharacterBase.h"
#include "MNpcBase.generated.h"

class UMNpcDefinition;
class UMInteractionComponent;

/**
 * 
 */
UCLASS(Abstract)
class PROJECTM_API AMNpcBase : public AMCharacterBase
{
	GENERATED_BODY()
/*
* Overrided Function
*/
public:
	AMNpcBase(const FObjectInitializer& ObjectInitializer);
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void InitCharacterName() override;

/*
* Member Functions
*/
public:
	const UMNpcDefinition* GetNpcDefinition() const { return NpcDefinition; }
	UMInteractionComponent* GetInteractionComponent() const { return InteractionComponent; }

#if WITH_EDITOR
	void SetNpcRowId(const int32 InRowId) { NpcRowId = InRowId; }
#endif
/*
* Member Variables
*/
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(), Category = "ProjectM")
	int32 NpcRowId = INDEX_NONE;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectM")
	TObjectPtr<UMNpcDefinition> NpcDefinition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectM")
	UMInteractionComponent* InteractionComponent;
};
