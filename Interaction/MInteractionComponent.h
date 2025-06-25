// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/SphereComponent.h"
#include "MInteractionComponent.generated.h"

class UInputComponent;
class UPMInputComponent;
class USphereComponent;
class UMInteractiveAction_OnInteractionBase;
class UMInteractiveAction_OverlapActionBase;
class APlayerController;
class AMPlayerCharacterBase;

UCLASS(meta = (BlueprintSpawnableComponent))
class PROJECTM_API UMInteractionComponent : public USphereComponent
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UMInteractionComponent(const FObjectInitializer& ObjectInitializer);
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

/*
* Member Functions
*/
public:
	void SetNewInteractions(const TArray<UMInteractiveAction_OverlapActionBase*>& OnBeginOverlap, const TArray<UMInteractiveAction_OnInteractionBase*>& OnInteract);

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	bool ExistActivatableAction() const;
	void OnActivatedInteractiveAction(UMInteractiveAction_OnInteractionBase* Action);
	void OnDeactivatedInteractiveAction(UMInteractiveAction_OnInteractionBase* Action);
	void OnAddedInteractionList();
	void OnRemovedInteractionList();

	const TArray<UMInteractiveAction_OverlapActionBase*>& GetOnBeginOverlapActions() const {return Action_OnBeginOverlap; }
	const TArray<UMInteractiveAction_OnInteractionBase*>& GetOnInteractActions() const {return Action_OnInteract; }

private:
	void ActivateAllOverlapAction();
	void DeactivateAllOverlapAction();

	void EnableOverlapAction(AActor* OtherActor);
	void DisableOverlapAction(AActor* OtherActor);
	void OnInteract(const FGameplayTag& Tag);
	void Callback_OnSetInputComponent(UInputComponent* InInputComponent);
	void BindDelegate();
	void UnbindDelegate();
	bool IsItServer() const;

	UPMInputComponent* GetInputComponent() const;
/*
* Member Variables
*/
protected:
	UPROPERTY(BlueprintReadOnly, Category = "ProjectM")
	TArray<TObjectPtr<UMInteractiveAction_OverlapActionBase>> Action_OnBeginOverlap;

	UPROPERTY(BlueprintReadOnly, Category = "ProjectM")
	TArray<TObjectPtr<UMInteractiveAction_OnInteractionBase>> Action_OnInteract;

	UPROPERTY(BlueprintReadOnly, Category = "ProjectM")
	TSet<UMInteractiveAction_OnInteractionBase*> ActivatedInteractiveActions;

	UPROPERTY(BlueprintReadOnly, Category = "ProjectM")
	bool bIsAddedInteractionList = false;

	TWeakObjectPtr<AMPlayerCharacterBase> WeakOverlappedCharacter;
};
