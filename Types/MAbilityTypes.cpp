#include "Types/MAbilityTypes.h"
#include "AbilitySystem/PMAbilitySystemComponent.h"

UE_DEFINE_GAMEPLAY_TAG(Animation_Notify_StartAttack, "Animation.Notify.StartAttack");
UE_DEFINE_GAMEPLAY_TAG(Animation_Notify_EndAttack, "Animation.Notify.EndAttack");
UE_DEFINE_GAMEPLAY_TAG(Animation_Notify_StartDead, "Animation.Notify.StartDead");
UE_DEFINE_GAMEPLAY_TAG(Animation_Notify_EndDead, "Animation.Notify.EndDead");
UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Combat_Hit, "GameplayCue.Combat.Hit");

void FMAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FMAbilitySet_GrantedHandles::TakeFromAbilitySystem(UPMAbilitySystemComponent* AbilitySystemComp)
{
	if (!AbilitySystemComp->IsOwnerActorAuthoritative())
	{
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			AbilitySystemComp->ClearAbility(Handle);
		}
	}
}

void FMAbilitySet_AppliedEffectHandles::AddAppliedEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	AppliedEffectHandles.Add(Handle);
}

void FMAbilitySet_AppliedEffectHandles::RemoveAppliedEffects(UPMAbilitySystemComponent* AbilitySystemComp)
{
	if (AbilitySystemComp)
	{
		for (const FActiveGameplayEffectHandle& Handle : AppliedEffectHandles)
		{
			AbilitySystemComp->RemoveActiveGameplayEffect(Handle);
		}

		AppliedEffectHandles.Empty();
	}
}

bool FMAbilitySet_AppliedEffectHandles::WasApplied() const
{
	return !AppliedEffectHandles.IsEmpty();
}

