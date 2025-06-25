#pragma once

#include "MCharacterBase.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "MPlayerCharacterBase.generated.h"

class UPMPawnExtensionComponent;
class UPMCameraComponent;
class UPMHealthComponent;
class UInputComponent;
class UPMAbilitySystemComponent;
class UMNavigationComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSetInputComponent, UInputComponent* PlayerInputComponent);

UCLASS()
class PROJECTM_API AMPlayerCharacterBase : public AMCharacterBase
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	AMPlayerCharacterBase(const FObjectInitializer& ObjectInitializer);
	virtual void OnDead() override;
	virtual void Restart() override;
	virtual void PawnClientRestart() override;
	virtual void BeginPlay() override;

	virtual void OnRep_PlayerState() override;
	virtual void InitCharacterName() override;

protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:
	virtual UAbilitySystemComponent*	GetAbilitySystemComponent() const override;
	UFUNCTION(BlueprintCallable)
	virtual UPMAbilitySystemComponent*	GetMAbilitySystemComponent() const override;

/*
* Member Functions
*/
public:
	void InitCharacterDefaultSpec();

	void Callback_OnAbilitySystemInitialized();
	void Callback_OnAbilitySystemUninitialzed();
	void CallOrRegister_OnSetInputComponent(FOnSetInputComponent::FDelegate&& Delegate);

	void AddOverlappedMonster(AActor* Monster);
	void RemoveOverlappedMonster(AActor* Monster);
	
	void SetCurrentLevelTag(const FGameplayTag& InCurrentLevelTag) { CurrentLevelTag = InCurrentLevelTag; }

	const TArray<AActor*>&	GetOverlappedMonsters() const { return OverlappedMonsters; }
	const FGameplayTag&		GetCurrentLevelTag() const { return CurrentLevelTag; }

protected:
	void ClearAbilityActorInfo();
	void InitAbilityActorInfo();

#if WITH_EDITOR
	UFUNCTION(Exec)
	void Test_KillPlayer();
	UFUNCTION(Server, Reliable)
	void Server_KillPlayer();
#endif
/*
* Member Variables
*/
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProjectM | Character")
	TObjectPtr<UPMPawnExtensionComponent> PawnExtComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectM | Character")
	TObjectPtr<UPMCameraComponent> CameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectM | Character")
	TObjectPtr<UPMHealthComponent> HealthComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UMNavigationComponent> NavigationComponent;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag CurrentLevelTag;

	UPROPERTY(BlueprintReadOnly)
	TArray<AActor*> OverlappedMonsters;

private:
	FOnSetInputComponent OnSetInputComponentDelegate;
};