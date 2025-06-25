#include "PMWeaponInstance.h"
#include "Character/MPlayerCharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Equipment/MEquipableActorBase.h"
#include "System/MDataTableManager.h"

UPMWeaponInstance::UPMWeaponInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UPMWeaponInstance::OnEquipped()
{
	Super::OnEquipped();

	const UMEquipmentItemDefinition* EquipDef = UMDataTableManager::GetDefinitionObject<UMEquipmentItemDefinition>(this, ItemRowId);
	if (EquipDef)
	{
		SpawnEquipmentActor(EquipDef->ActorToSpawn);
	}
}

void UPMWeaponInstance::OnUnequipped()
{
	Super::OnUnequipped();

	DestroySpawnedActor();
}

void UPMWeaponInstance::SpawnEquipmentActor(const FPMEquipmentActorToSpawn& ActorInfo)
{
	APawn* OwningPawn = GetPawn();
	if (OwningPawn)
	{
		if (ActorInfo.ActorToSpawn)
		{
			USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
			if (ACharacter* Character = Cast<ACharacter>(OwningPawn))
			{
				AttachTarget = Character->GetMesh();
			}

			AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(ActorInfo.ActorToSpawn, FTransform::Identity, OwningPawn);
			NewActor->FinishSpawning(FTransform::Identity, true);

			NewActor->SetActorRelativeTransform(ActorInfo.AttachTransform);

			NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, ActorInfo.AttachSocket);
			AMEquipableActorBase* EquippableActor = Cast<AMEquipableActorBase>(NewActor);
			if (EquippableActor)
			{
				EquippableActor->SetItemDef(ItemDef.Get());
			}

			SpawnedActor = NewActor;
		}
		else
		{
			ensure(false);
			MCHAE_ERROR("장비 테이블에 스폰할 액터가 설정되어있지 않음.");
		}
	}
	else
	{
		ensure(false);
		// 왜 Pawn이 없지?
	}
}

void UPMWeaponInstance::DestroySpawnedActor()
{
	if (IsValid(SpawnedActor))
	{
		SpawnedActor->Destroy();
		SpawnedActor = nullptr;
	}
}
