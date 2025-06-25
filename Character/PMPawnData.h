#pragma once

#include "Engine/DataAsset.h"
#include "PMPawnData.generated.h"

class UPMCameraMode;
class UPMInputConfig;
class UPMAbilitySet;
class UPMInventoryItemDefinition;
class UGameplayEffect;
class UAnimMontage;
class UMEquipmentItemDefinition;
class UMGameplayEffectSet;

UCLASS()
class PROJECTM_API UPMPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
/*
* Overrided Function
*/
public:
	UPMPawnData();
	
/*
* Member Functions
*/
public:
	const TSubclassOf<APawn>&					GetPawnClass() const { return PawnClass; }
	const TSubclassOf<UPMCameraMode>&			GetDefaultCameraMode() const { return DefaultCameraMode; }
	const UPMInputConfig*						GetInputConfig() const { return InputConfig; }
	const TArray<TObjectPtr<UPMAbilitySet>>&	GetAbilitySets() const { return AbilitySets; }

/*
* Member Variables
*/
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectM | Pawn")
	TSubclassOf<APawn> PawnClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectM | Pawn")
	TObjectPtr<UAnimMontage> DeathAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectM | Camera")
	TSubclassOf<UPMCameraMode> DefaultCameraMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectM | Input")
	TObjectPtr<UPMInputConfig> InputConfig;

	// Character가 기본적으로 사용하게 될 Ability
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectM | Abilities")
	TArray<TObjectPtr<UPMAbilitySet>> AbilitySets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectM | Abilities")
	TArray<TObjectPtr<UMGameplayEffectSet>> EffectSets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectM | Equipment")
	int32 DefaultEquipmentRowId;
};