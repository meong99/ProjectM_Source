// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/TriggerBox.h"
#include "Animation/WidgetAnimationEvents.h"
#include "MLevelTravelingActor.generated.h"

class UBoxComponent;
class UPrimitiveComponent;
class UMLevelManager;
struct FMTable_MapTable;
class AMPlayerCharacterBase;

/**
 *
 */
UCLASS()
class PROJECTM_API AMLevelTravelingActor : public ATriggerBox
{
	GENERATED_BODY()
/*
* Overrided Function
*/
public:
	AMLevelTravelingActor();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

/*
* Member Functions
*/
public:
	UFUNCTION()
	void OnBeginOverlap_LevelTravel(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	void	PlayLoadingFadeOut(FWidgetAnimationDynamicEvent&& Callback) const;
	void	PlayLoadingFadeIn(FWidgetAnimationDynamicEvent&& Callback) const;

	UFUNCTION()
	void	OnFinishedFadeOut();
	UFUNCTION()
	void	OnFinishedFadeIn();

	AActor* FindDestPlayerStart() const;
	AActor* FindDestTravelActor() const;
	void	TeleportToDestination();
	void	SetPlayerCollisionToTravel() const;
	void	SetPlayerCollisionToOrigin() const;
	void	RequestOngoingNavigation() const;

/*
* Member Variables
*/
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ProjectM")
	FGameplayTag DestLevelTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ProjectM")
	FGameplayTag OriginLevelTag;

	const FMTable_MapTable* LevelInfo;

	UPROPERTY()
	TObjectPtr<AMPlayerCharacterBase> PlayerCharacter;
};
