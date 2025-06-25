#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MEquipmentItemDefinition.h"
#include "Inventory/PMInventoryItemInstance.h"
#include "Types/MAbilityTypes.h"
#include "MEquipmentItemInstance.generated.h"

class UPMAbilitySystemComponent;
class UAnimMontage;

/**
 * EquipmentManager에서 관리하는 장착중인 장비에 대한 Instance
 */
UCLASS(BlueprintType, Blueprintable)
class PROJECTM_API UMEquipmentItemInstance : public UPMInventoryItemInstance
{
	GENERATED_BODY()

	/*
	* Overrided Functions
	*/
public:
	UMEquipmentItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override { return true; }

	virtual void OnInstanceCreated() override;
	virtual bool CanUseItem() const override;

/*
* Member Functions
*/
public:
	virtual void	OnEquipped();
	virtual void	OnUnequipped();

	UFUNCTION(BlueprintPure, Category = "Equipment")
	UObject*	GetInstigator() { return this; }

	UFUNCTION(BlueprintPure, Category = "Equipment")
	APawn*		GetPawn() const;

	UFUNCTION(BlueprintPure, Category = "Equipment")
	AActor*		GetSpawnedActor() const { return SpawnedActor; }

	EMEquipmentItemType	GetEquipmentItemType() const { return EquipmentItemType; }

protected:
	UPMAbilitySystemComponent* GetAbilitySystemComponent() const;

/*
* Member Variables
*/
public:
	// EquipmentDefinition에 따라서 Instancing된 Actor들
	UPROPERTY(Replicated)
	TObjectPtr<AActor> SpawnedActor;

	UPROPERTY(VisibleAnywhere)
	EMEquipmentItemType EquipmentItemType = EMEquipmentItemType::None;

	UPROPERTY()
	FMAbilitySet_AppliedEffectHandles AppliedEffectHandles;
};