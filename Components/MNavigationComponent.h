// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/MPawnComponentBase.h"
#include "GameplayTagContainer.h"
#include "Containers/Queue.h"
#include "MNavigationComponent.generated.h"

class UArrowComponent;
class UNavigationPath;
class AMPlayerCharacterBase;

/**
 *
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class PROJECTM_API UMNavigationComponent : public UMPawnComponentBase
{
	GENERATED_BODY()

	/*
	* Overrided Function
	*/
public:
	UMNavigationComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;

	/*
	* Member Functions
	*/
public:
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_ActivateNavigation(const FGameplayTag& SearchTag, const FGameplayTag& OriginLevelTag);
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_DeactivateNavigation();

	void RequestOngoingNavigation();

private:
	UFUNCTION()
	void	OnRep_MovementInput();

	void	SetMovementInput(const FVector& NewMovementInput);
	void	GeneratePathData();
	void	StopAndCheckDest();

	void	ShowPathPointDebugLine();
	FVector	PopDestLocation();
	FVector	GetFloorLocation(AActor* TargetActor, bool& bHit) const;
	/*
	* Member Variables
	*/
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectM")
	float GuideSpeed = 1.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectM")
	float DistanceThreshold = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectM")
	float GoalThreshold = 300.f;

	UPROPERTY(ReplicatedUsing=OnRep_MovementInput)
	FVector MovementInput;

	UPROPERTY()
	TObjectPtr<UNavigationPath> Path;

	UPROPERTY()
	AMPlayerCharacterBase* OwnerCharacter;

	int32 CurrentPathIndex = 0;

	TQueue<FGameplayTag> DestTags;
};
