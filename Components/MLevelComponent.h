// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MPlayerStateComponentBase.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Table/MTable_LevelTable.h"
#include "Types/MAbilityTypes.h"
#include "MLevelComponent.generated.h"

class UPMAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnChange_Level, const int32 OldLevel, const int32 NewLevel, const int64 MaxExp);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnChange_Exp, const int64 OldExp, const int64 NewExp);

/**
 *
 */
UCLASS(BlueprintType)
class PROJECTM_API UMLevelComponent : public UMPlayerStateComponentBase
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UMLevelComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	/*
* Member Functions
*/
public:
	FDelegateHandle	CallAndRegister_OnChangeLevel(FOnChange_Level::FDelegate&& Delegate);
	FDelegateHandle	CallAndRegister_OnChangeExp(FOnChange_Exp::FDelegate&& Delegate);
	void			RemoveDelegate_OnChangeLevel(const FDelegateHandle& Handle);
	void			RemoveDelegate_OnChangeExp(const FDelegateHandle& Handle);

private:
	void	InitLevel();
	void	SetMaxExperiencePoint(const int32 Level);
	void	OnChange_ExperiencePoint(FGameplayTag Tag, const FGameplayEventData* EventData);
	void	LevelUp();
	void	ApplyLevelUpStat();
	float	CalculateAdditiveStatByLevel(const FGameplayTag& StatTag, const float DefaultValue);

	UFUNCTION()
	void OnRep_OnChangeLevel(const int32 OldLevel);
	UFUNCTION()
	void OnRep_OnChangeExperience(const int64 OldExperiencePoint);
	void BroadcastOnChangeLevel(const int32 OldLevel);
	void BroadcastOnChangeExp(const int64 OldExperiencePoint);

	const int32 GetCurrentLevel() const { return CurrentLevel; }
	const int64 GetCurrentExp() const { return CurrentExperiencePoint; }
	const int64 GetCurrentMaxExp() const { return CurrentMaxExperiencePoint; }

	UFUNCTION(Exec)
	void Debug_LevelUp();
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Debug_LevelUpServer();
/*
* Member Variables
*/
protected:
	FOnChange_Level	Delegate_OnChangeLevel;
	FOnChange_Exp	Delegate_OnChangeExp;

protected:
	UPROPERTY(ReplicatedUsing = "OnRep_OnChangeLevel", BlueprintReadOnly, meta = (ClampMin = 0), Category = "ProjectM")
	int32	CurrentLevel = 1;

	UPROPERTY(ReplicatedUsing = "OnRep_OnChangeExperience", BlueprintReadOnly, meta = (ClampMin = 0), Category = "ProjectM")
	int64	CurrentExperiencePoint = 0;

	UPROPERTY(BlueprintReadOnly, meta = (ClampMin = 0.0), Category="ProjectM")
	int64	CurrentMaxExperiencePoint = 1;

	UPROPERTY()
	UPMAbilitySystemComponent* AbilitySystemComp;

	FMTable_LevelTable* CurrentLevelTableRow;

	FMAbilitySet_AppliedEffectHandles AppliedEffectHandles;
};
