#pragma once

#include "CoreMinimal.h"
#include "PMWeaponInstance.h"
#include "PMGameplayTags.h"
#include "PMRangedWeaponInstance.generated.h"

/**
 *
 */
UCLASS()
class PROJECTM_API UPMRangedWeaponInstance : public UPMWeaponInstance
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UPMRangedWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

/*
* Member Functions
*/
public:

/*
* Member Variables
*/
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ForceUnits = cm),  Category = "WeaponConfig")
	float MaxDamageRange = 25000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ForceUnits = cm),  Category = "WeaponConfig")
	float BulletTraceWeaponRadius = 0.0f;
};