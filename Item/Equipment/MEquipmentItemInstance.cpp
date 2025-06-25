#include "MEquipmentItemInstance.h"
#include "PMGameplayTags.h"
#include "Player/PMPlayerControllerBase.h"
#include "AbilitySystem/PMAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "MEquipmentItemDefinition.h"
#include "GameplayTagContainer.h"
#include "Player/PMPlayerState.h"
#include "AbilitySystem/MGameplayEffectSet.h"

UMEquipmentItemInstance::UMEquipmentItemInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMEquipmentItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMEquipmentItemInstance, SpawnedActor);
}

void UMEquipmentItemInstance::OnInstanceCreated()
{
	UMEquipmentItemDefinition* ItemCDO = ItemDef->GetDefaultObject<UMEquipmentItemDefinition>();
	if (ItemCDO)
	{
		EquipmentItemType = ItemCDO->EquipmentItemType;
	}
}

bool UMEquipmentItemInstance::CanUseItem() const
{
	bool bCanUseItem = false;

	if (GetStatTagStackCount(FPMGameplayTags::Get().Item_Quentity))
	{
		bCanUseItem = true;
	}
	if (GetAbilitySystemComponent())
	{
		bCanUseItem &= true;
	}

	return bCanUseItem;
}

void UMEquipmentItemInstance::OnEquipped()
{
	const UMEquipmentItemDefinition* ItemDefCDO = GetDefault<UMEquipmentItemDefinition>(ItemDef);
	UPMAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ItemDefCDO || !ASC)
	{
		return;
	}

	for (const UMGameplayEffectSet* EffectSet : ItemDefCDO->EffectSet)
	{
		if (EffectSet)
		{
			EffectSet->ApplyGameplayEffectsToAsc(ASC, &AppliedEffectHandles, Cast<AActor>(GetOuter()), nullptr, this);
		}
		else
		{
			MCHAE_ERROR("EffectSet is not defined! check item definition! Definition name is %s", *ItemDefCDO->GetName());
		}
	}
}

void UMEquipmentItemInstance::OnUnequipped()
{
	UPMAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (ASC)
	{
		AppliedEffectHandles.RemoveAppliedEffects(ASC);
	}
}

APawn* UMEquipmentItemInstance::GetPawn() const
{
	APMPlayerState* PlayerState = Cast<APMPlayerState>(GetOuter());
	if (PlayerState)
	{
		return PlayerState->GetPawn();
	}

	return nullptr;
}

UPMAbilitySystemComponent* UMEquipmentItemInstance::GetAbilitySystemComponent() const
{
	APMPlayerState* PlayerState = Cast<APMPlayerState>(GetOuter());
	if (PlayerState)
	{
		return PlayerState->GetPMAbilitySystemComponent();
	}

	return nullptr;
}
