#pragma once

#include "CoreMinimal.h"
#include "Inventory/PMInventoryItemInstance.h"
#include "GameFramework/Pawn.h"
#include "MEquipmentItemDefinition.h"
#include "MEquipableItemInstance.generated.h"

class UPMAbilitySystemComponent;
class UAnimMontage;
class APlayerState;

/**
 *	Inventory에서 아직 장착하지 않은 장비들의 Instance.
 */
UCLASS(BlueprintType, Blueprintable)
class PROJECTM_API UMEquipableItemInstance : public UPMInventoryItemInstance
{
	GENERATED_BODY()

	/*
	* Overrided Functions
	*/
public:
	UMEquipableItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual bool	IsSupportedForNetworking() const override { return true; }
	virtual void	OnInstanceCreated() override;
	virtual bool	ActivateItem() override;
	virtual bool	CanUseItem() const override;

/*
* Member Functions
*/
public:
	UFUNCTION(BlueprintPure, Category = "Equipment")
	UObject*				GetInstigator() { return this; }

	UFUNCTION(BlueprintPure, Category = "Equipment")
	APlayerState*			GetPlayerState() const;

	EMEquipmentItemType		GetEquipmentItemType() const { return EquipmentItemType; }

protected:
	UPMAbilitySystemComponent* GetAbilitySystemComponent() const;

/*
* Member Variables
*/
public:
	UPROPERTY(VisibleAnywhere)
	EMEquipmentItemType EquipmentItemType = EMEquipmentItemType::None;
};