#pragma once

#include "CoreMinimal.h"
#include "Equipment/MEquipableActorBase.h"
#include "MWeaponBase.generated.h"

class UCapsuleComponent;
class UPrimitiveComponent;
class UPMAbilitySystemComponent;
class USkeletalMeshComponent;

/**
 *
 */
UCLASS()
class PROJECTM_API AMWeaponBase : public AMEquipableActorBase
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	AMWeaponBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void PostInitializeComponents() override;

	/*
* Member Functions
*/
public:
	UFUNCTION(BlueprintCallable)
	virtual void ActivateWeapon();

	UFUNCTION(BlueprintCallable)
	virtual void DeactivateWeapon();

protected:
	UPMAbilitySystemComponent* GetAbilitySystemComponent() const;

/*
* Member Variables
*/
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	USkeletalMeshComponent* WeaponMesh;
};