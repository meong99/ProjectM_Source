#include "MConsumableItemInstance.h"
#include "PMGameplayTags.h"
#include "MConsumableItemDefinition.h"
#include "Player/PMPlayerControllerBase.h"
#include "AbilitySystem/PMAbilitySystemComponent.h"
#include "AbilitySystem/MGameplayEffectSet.h"
#include "GameFramework/Actor.h"

UMConsumableItemInstance::UMConsumableItemInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

bool UMConsumableItemInstance::ActivateItem()
{
	const bool bIsActivated = Super::ActivateItem();

	if (!bIsActivated)
	{
		return bIsActivated;
	}

	const UMConsumableItemDefinition* ItemDefCDO = GetDefault<UMConsumableItemDefinition>(ItemDef);
	UPMAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ItemDefCDO || !ASC)
	{
		MCHAE_ERROR("Can't Find Item Information or not Accessable to Asc");
		check(false);
	}

	for (const UMGameplayEffectSet* EffectSet : ItemDefCDO->EffectSet)
	{
		if (EffectSet)
		{
			FMAbilitySet_AppliedEffectHandles TempHandles;
			EffectSet->ApplyGameplayEffectsToAsc(ASC, &TempHandles, Cast<AActor>(GetOuter()), nullptr, this);
		}
		else
		{
			MCHAE_ERROR("EffectSet is not defined! check item definition! Definition name is %s", *ItemDefCDO->GetName());
		}
	}
		
	return bIsActivated;
}

bool UMConsumableItemInstance::CanUseItem() const
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

UPMAbilitySystemComponent* UMConsumableItemInstance::GetAbilitySystemComponent() const
{
	APMPlayerControllerBase* Controller = Cast<APMPlayerControllerBase>(GetOuter());
	if (Controller)
	{
		return Controller->GetAbilitySystemComponent();
	}

	return nullptr;
}
