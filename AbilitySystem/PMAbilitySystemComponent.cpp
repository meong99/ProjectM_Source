#include "PMAbilitySystemComponent.h"
#include "Abilities/PMGameplayAbility.h"
#include "Animation/PMAnimInstance.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"

UPMAbilitySystemComponent::UPMAbilitySystemComponent()
{
	SetIsReplicatedByDefault(true);
}

void UPMAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	check(ActorInfo);
	check(InOwnerActor);

	const bool bHasNewPawnAvatar = Cast<APawn>(InAvatarActor) && InAvatarActor != ActorInfo->AvatarActor;

	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if (bHasNewPawnAvatar)
	{
		if (UPMAnimInstance* AnimInst = Cast<UPMAnimInstance>(ActorInfo->GetAnimInstance()))
		{
			AnimInst->InitializeWithAbilitySystem(this);
		}
	}
}

void UPMAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	if (Spec.IsActive())
	{
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UPMAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);
	
	if (Spec.IsActive())
	{
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

AActor* UPMAbilitySystemComponent::GetInstigatorFromAttrChangeData(const FOnAttributeChangeData& ChangeData)
{
	if (ChangeData.GEModData != nullptr)
	{
		const FGameplayEffectContextHandle& EffectContext = ChangeData.GEModData->EffectSpec.GetEffectContext();
		return EffectContext.GetOriginalInstigator();
	}
	return nullptr;
}

void UPMAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
			{
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpenHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}
}

void UPMAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
			{
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpenHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}

void UPMAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;

	// 입력 홀드 관련
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpenHandles)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				const UPMGameplayAbility* AbilityCDO = CastChecked<UPMGameplayAbility>(AbilitySpec->Ability);

				if (AbilityCDO->GetActivationPolicy() == EPMAbilityActivationPolicy::WhileInputActive)
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}

	// 입력 눌렀을 때 트리거 관련
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;

				UPMGameplayAbility* Ability = Cast<UPMGameplayAbility>(AbilitySpec->Ability);
				if (Ability && !Ability->CanRetrigger() && AbilitySpec->IsActive())
				{
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					const UPMGameplayAbility* AbilityCDO = CastChecked<UPMGameplayAbility>(AbilitySpec->Ability);

					if (AbilityCDO->GetActivationPolicy() == EPMAbilityActivationPolicy::OnInputTriggered)
					{
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					}
				}
			}
		}
	}

	// 어빌리티 실행
	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(AbilitySpecHandle);
	}

	// 입력 Release 관련
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;
				if (AbilitySpec->IsActive())
				{
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}

	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

FGameplayEffectSpec UPMAbilitySystemComponent::MakeGameplayEffectSpecWithSetByCaller(const FGameplayEffectContextHandle& ContextHandle, TSubclassOf<UGameplayEffect> EffectClass, 
																					 TMap<FGameplayTag, float> SetbyCallerMap, const FGameplayTag& DynamicTag)
{
	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(EffectClass, 1, ContextHandle);
	FGameplayEffectSpec* Spec = EffectSpecHandle.Data.Get();
	if (Spec == nullptr)
	{
		MCHAE_WARNING("EffectSpec is not valid");
		return {};
	}

	for (const auto& Iter : SetbyCallerMap)
	{
		Spec->SetSetByCallerMagnitude(Iter.Key, Iter.Value);
	}

	Spec->AddDynamicAssetTag(DynamicTag);

	return *Spec;
}

FGameplayEffectContextHandle UPMAbilitySystemComponent::MakeGameplayEffectContext(AActor* Instigator, AActor* EffectCauser, const FHitResult& HitResult)
{
	FGameplayEffectContextHandle ContextHandle = MakeEffectContext();
	ContextHandle.AddInstigator(Instigator, EffectCauser);
	ContextHandle.AddHitResult(HitResult);

	return ContextHandle;
}

void UPMAbilitySystemComponent::SendGameplayTagToAbility(const FGameplayTag& InputTag, const FGameplayTag& SendTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
			{
				UPMGameplayAbility* Ability = Cast<UPMGameplayAbility>(AbilitySpec.Ability);
				if (Ability)
				{
					Ability->OnReceivedGameplayTag(SendTag);
				}
				else
				{
					ensure(false);
				}
				break;
			}
		}
	}
}
