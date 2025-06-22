// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/MShopTypes.h"
#include "MInteractiveAction_OnInteractionBase.h"
#include "MInteractiveAction_OpenShop.generated.h"

class UInputComponent;

UCLASS()
class PROJECTM_API UMInteractiveAction_OpenShop : public UMInteractiveAction_OnInteractionBase
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UMInteractiveAction_OpenShop(const FObjectInitializer& ObjectInitializer);

	virtual void InitAction(UMInteractionComponent* InInteractionComponent, AActor* InOwnerActor) override;
	virtual void ActivateAction() override;
	virtual void DeactivateAction() override;

/*
* Member Functions
*/
public:
	const FMShopDefinition& GetShopDefinition() const { return ShopDefinition; }

/*
* Member Variables
*/
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(), Category = "ProjectM")
	int32 ShopRowId = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category = "ProjectM")
	bool bIsActivated = false;

	UPROPERTY(BlueprintReadOnly, Category = "ProjectM")
	FMShopDefinition ShopDefinition;
};
