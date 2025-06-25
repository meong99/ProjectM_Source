#include "MGameplayEffectSet.h"
#include "PMAbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameFramework/Actor.h"

UMGameplayEffectSet::UMGameplayEffectSet()
{
}

void UMGameplayEffectSet::ApplyGameplayEffectsToAsc(UPMAbilitySystemComponent* AbilitySystemComp, OUT FMAbilitySet_AppliedEffectHandles* OutAppliedEffectHandles, AActor* Instigator, AActor* EffectCausor, UObject* SourceObject) const
{
	check(AbilitySystemComp);

	if (!AbilitySystemComp->IsOwnerActorAuthoritative())
	{
		return;
	}

	for (int32 Index = 0; Index < EffectToApply.Num(); ++Index)
	{
		const FMAbilitySet_GameplayEffect& EffectSet = EffectToApply[Index];
		if (!IsValid(EffectSet.Effect))
		{
			continue;
		}
		FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComp->MakeGameplayEffectContext(Instigator, EffectCausor);
		EffectContextHandle.AddSourceObject(SourceObject);

		TMap<FGameplayTag, float> SetbyCallerMap;
		for (const FMSetbyCallerFloat& Value : EffectSet.SetbyCallers)
		{
			if (Value.SetByCallerTag.IsValid())
			{
				SetbyCallerMap.Add(Value.SetByCallerTag, Value.Value);
			}
		}

		const FGameplayEffectSpec& Spec = AbilitySystemComp->MakeGameplayEffectSpecWithSetByCaller(EffectContextHandle, EffectSet.Effect, SetbyCallerMap);
		FActiveGameplayEffectHandle AppliedEffectHandle = AbilitySystemComp->ApplyGameplayEffectSpecToSelf(Spec);
		if (AppliedEffectHandle.IsValid() && OutAppliedEffectHandles)
		{
			OutAppliedEffectHandles->AddAppliedEffectHandle(AppliedEffectHandle);
		}
	}
}

void UMGameplayEffectSet::ApplyGameplayEffectsToAsc(UPMAbilitySystemComponent* AbilitySystemComp, OUT FMAbilitySet_AppliedEffectHandles* OutAppliedEffectHandles, TFunctionRef<float(const FGameplayTag& SetTag, const float DefaultValue)> Calculate, AActor* Instigator, AActor* EffectCausor, UObject* SourceObject) const
{
	check(AbilitySystemComp);

	if (!AbilitySystemComp->IsOwnerActorAuthoritative())
	{
		return;
	}

	for (int32 Index = 0; Index < EffectToApply.Num(); ++Index)
	{
		const FMAbilitySet_GameplayEffect& EffectSet = EffectToApply[Index];
		if (!IsValid(EffectSet.Effect))
		{
			continue;
		}
		FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComp->MakeGameplayEffectContext(Instigator, EffectCausor);
		EffectContextHandle.AddSourceObject(SourceObject);

		TMap<FGameplayTag, float> SetbyCallerMap;
		for (const FMSetbyCallerFloat& Value : EffectSet.SetbyCallers)
		{
			if (Value.SetByCallerTag.IsValid())
			{
				SetbyCallerMap.Add(Value.SetByCallerTag, Calculate(Value.SetByCallerTag, Value.Value));
			}
		}

		const FGameplayEffectSpec& Spec = AbilitySystemComp->MakeGameplayEffectSpecWithSetByCaller(EffectContextHandle, EffectSet.Effect, SetbyCallerMap);
		FActiveGameplayEffectHandle AppliedEffectHandle = AbilitySystemComp->ApplyGameplayEffectSpecToSelf(Spec);
		if (AppliedEffectHandle.IsValid() && OutAppliedEffectHandles)
		{
			OutAppliedEffectHandles->AddAppliedEffectHandle(AppliedEffectHandle);
		}
	}
}
