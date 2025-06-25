#include "PMAbilitySet.h"
#include "PMAbilitySystemComponent.h"
#include "Abilities/PMGameplayAbility.h"

/*
* UPMAbilitySet --------------------------------------
*/
UPMAbilitySet::UPMAbilitySet()
{
}

void UPMAbilitySet::GiveToAbilitySystem(UPMAbilitySystemComponent* AbilitySystemComp, OUT FMAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject, int32 AdditiveInfo) const
{
	check(AbilitySystemComp);

	if (!AbilitySystemComp->IsOwnerActorAuthoritative())
	{
		return;
	}

	// 부여해야할 모든 어빌리티를 돌면서 확인
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		// 어빌리티가 지정되어있는지 확인. 에디터에서 null로 넣어놓을 수 있기 때문.
		const FPMAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];
		if (!IsValid(AbilityToGrant.Ability))
		{
			continue;
		}

		UPMGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UPMGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);
		AbilitySpec.InputID = AdditiveInfo;

		const FGameplayAbilitySpecHandle AbilitySpecHandle = AbilitySystemComp->GiveAbility(AbilitySpec);
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}
}
