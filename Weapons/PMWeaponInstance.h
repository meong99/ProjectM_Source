#pragma once

#include "CoreMinimal.h"
#include "Item/Equipment/MEquipableItemInstance.h"
#include "Cosmetics/PMCosmeticAnimationTypes.h"
#include "Animation/AnimMontage.h"
#include "Item/Equipment/MEquipmentItemInstance.h"

#include "PMWeaponInstance.generated.h"

class UAnimInstance;

/**
 *
 */
UCLASS()
class PROJECTM_API UPMWeaponInstance : public UMEquipmentItemInstance
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UPMWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void	OnEquipped() override;
	virtual void	OnUnequipped() override;
/*
* Member Functions
*/
protected:
	void	SpawnEquipmentActor(const FPMEquipmentActorToSpawn& ActorInfo);
	void	DestroySpawnedActor();
/*
* Member Variables
*/
};