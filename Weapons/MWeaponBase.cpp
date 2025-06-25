#include "MWeaponBase.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystem/PMAbilitySystemComponent.h"
#include "PMGameplayTags.h"
#include "Item/Equipment/MWeaponItemDefinition.h"
#include "Character/Monster/MMonsterBase.h"
#include "Util/MGameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"

AMWeaponBase::AMWeaponBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;

	bReplicates = true;
}

void AMWeaponBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AMWeaponBase::ActivateWeapon()
{
}

void AMWeaponBase::DeactivateWeapon()
{
}

UPMAbilitySystemComponent* AMWeaponBase::GetAbilitySystemComponent() const
{
	AMCharacterBase* Character = GetOwner<AMCharacterBase>();
	if (Character)
	{
		return Character->GetMAbilitySystemComponent();
	}

	return nullptr;
}

