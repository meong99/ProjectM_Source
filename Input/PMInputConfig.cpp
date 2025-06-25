#include "PMInputConfig.h"

UPMInputConfig::UPMInputConfig(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

const UInputAction* UPMInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FPMInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && Action.InputTag == InputTag)
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		MCHAE_WARNING("can't find NaticeInputAction for InputTag, [%s] on InputConfig [%s].", *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

const UInputAction* UPMInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FPMInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag == InputTag)
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		MCHAE_WARNING("can't find AbilityInputActions for InputTag, [%s] on InputConfig [%s].", *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

const UInputAction* UPMInputConfig::FindToggleInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound /*= true*/) const
{
	for (const FPMInputAction& Action : ToggleInputActions)
	{
		if (Action.InputAction && Action.InputTag == InputTag)
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		MCHAE_WARNING("can't find ToggleInputActions for InputTag, [%s] on InputConfig [%s].", *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}
