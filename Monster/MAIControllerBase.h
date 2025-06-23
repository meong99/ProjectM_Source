#pragma once

#include "GameFramework/Actor.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "MAIControllerBase.generated.h"

class UMMonsterDefinition;
class UAISenseConfig_Sight;
class UBehaviorTree;
class UAISenseConfig_Damage;
class AMMonsterBase;
class UPMAbilitySystemComponent;

UCLASS()
class PROJECTM_API AMAIControllerBase : public AAIController
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	AMAIControllerBase();

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
/*
* Member Functions
*/
public:
	void	OnChange_CharacterStateFlag(const int64& OldFlag, const int64& NewFlag);

	UFUNCTION()
    void    OnTargetDetectedDelegated(AActor* Actor, struct FAIStimulus Stimulus);
	UFUNCTION()
    void    OnTargetForgotDelegated(AActor* Actor);

protected:
	void SetBehaviorTree(AMMonsterBase* Monster);
	void BindTargetDeathCallback();
	void RemoveTargetDeathCallback();
	void OnTargetDead(FGameplayTag Tag, const FGameplayEventData* EventData);
/*
* Member Variables
*/
protected:
	UPROPERTY()
	TObjectPtr<UAISenseConfig_Damage> DamageConfig;

	FDelegateHandle DelegateHandle_MonsterStateChange;
	FDelegateHandle DelegateHandle_TargetLiftStateChange;

	UPROPERTY()
	TObjectPtr<UPMAbilitySystemComponent> AbilitySystemComponent;
};