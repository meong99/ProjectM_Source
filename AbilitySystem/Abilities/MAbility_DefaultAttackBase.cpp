#include "MAbility_DefaultAttackBase.h"
#include "Weapons/PMWeaponInstance.h"
#include "Weapons/MWeaponBase.h"
#include "System/MDataTableManager.h"
#include "Engine/Engine.h"
#include "Item/Equipment/MWeaponItemDefinition.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystem/AbilityTasks/MAbilityTask_SphereTracer.h"
#include "AbilitySystem/AbilityTasks/MAbilityTask_RotateToActor.h"
#include "AbilitySystem/Attributes/PMCombatSet.h"
#include "AbilitySystem/PMAbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/Monster/MMonsterBase.h"
#include "PMGameplayTags.h"
#include "Equipment/PMEquipmentManagerComponent.h"
#include "GameplayCueFunctionLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Character/MPlayerCharacterBase.h"

UMAbility_DefaultAttackBase::UMAbility_DefaultAttackBase()
{

}

void UMAbility_DefaultAttackBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMAbility_DefaultAttackBase, bCanCombo);
}

bool UMAbility_DefaultAttackBase::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags /*= nullptr*/, const FGameplayTagContainer* TargetTags /*= nullptr*/, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr*/) const
{
	bool bCanActivate = Super::CanActivateAbility(Handle, ActorInfo, nullptr, nullptr, nullptr);
	bCanActivate &= bCanCombo;

	return bCanActivate;
}

void UMAbility_DefaultAttackBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec();
	UMDataTableManager* TableManager = GEngine->GetEngineSubsystem<UMDataTableManager>();
	ItemDef = TableManager ? Cast<UMWeaponItemDefinition>(TableManager->GetItemDefinition(Spec->InputID)) : nullptr;

	if (ItemDef)
	{
		if (!PlayMontage())
		{
			NotifyMontageEndCallBack();
			return;
		}

		if (HasAuthority(&ActivationInfo))
		{
			if (!BindAnimNotify())
			{
				NotifyMontageEndCallBack();
				return;
			}
		}

		AActor* Nearlest = FindNearlestTarget();
		RotateToTarget(Nearlest);
	}
	else
	{
		NotifyMontageEndCallBack();
	}
}

void UMAbility_DefaultAttackBase::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	OverlappedActors.Empty();
	ItemDef = nullptr;
}

void UMAbility_DefaultAttackBase::StartAttackTracing(FGameplayEventData Payload)
{
	FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec();
	UPMWeaponInstance* WeaponInstance = Cast<UPMWeaponInstance>(Spec->SourceObject);
	const FGameplayAbilityActivationInfo ActivationInfo = GetCurrentActivationInfo();
	if (HasAuthority(&ActivationInfo) && WeaponInstance)
	{
		ACharacter* OwnerCharacter = Cast<ACharacter>(WeaponInstance->GetPawn());
		WeakWeaponActor = WeaponInstance->GetSpawnedActor();
		if (OwnerCharacter)
		{
			TraceAttack(OwnerCharacter);
		}
	}
}

void UMAbility_DefaultAttackBase::EndAttackTracing(FGameplayEventData Payload)
{
	bCanCombo = true;
	if (TraceTask)
	{
		TraceTask->EndTask();
		TraceTask = nullptr;
	}
}

void UMAbility_DefaultAttackBase::TraceAttack(ACharacter* OwnerCharacter)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = { UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1) };
	TArray<AActor*> ActorsToIgnore = { OwnerCharacter };
	EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;
	TArray<FHitResult> OutHits;

	if (!WeakWeaponActor.IsValid() || !WeakWeaponActor->FindComponentByClass<USkeletalMeshComponent>())
	{
		ensure(false);
		MCHAE_WARNING("Ability trace fail!! Because can't found weapon actor! Weapon actor must be spawned! ");
		return;
	}

	UMAbilityTask_SphereTracer* Task = UMAbilityTask_SphereTracer::CreateSphereTracerTask(
		this, 
		WeakWeaponActor->FindComponentByClass<USkeletalMeshComponent>(),
		30.f,
		TEXT("weapon_start"),
		TEXT("weapon_end"),
		ObjectTypes, 
		FOnHit::TDelegate::CreateUObject(this, &UMAbility_DefaultAttackBase::Callback_OnHit), 
		ActorsToIgnore
	);

	if (Task)
	{
		Task->ReadyForActivation();
		TraceTask = Task;
	}
	else
	{
		ensure(false);
		MCHAE_WARNING("Can't create SphereTracer!!");
	}
}

void UMAbility_DefaultAttackBase::Callback_OnHit(const TArray<FHitResult>& HitResults)
{
	UPMAbilitySystemComponent* OwnerAbilitySystem = Cast<UPMAbilitySystemComponent>(GetCurrentActorInfo()->AbilitySystemComponent);
	for (const FHitResult& HitResult : HitResults)
	{
		ApplyEffectToTarget(OwnerAbilitySystem, HitResult);
	}
}

void UMAbility_DefaultAttackBase::ApplyEffectToTarget(UPMAbilitySystemComponent* OwnerAbilitySystem, const FHitResult& HitResult)
{
	AActor* HitActor = HitResult.GetActor();
	AMMonsterBase* Monster = Cast<AMMonsterBase>(HitActor);
	if (Monster && !OverlappedActors.Contains(HitActor) && ItemDef)
	{
		UPMAbilitySystemComponent* MonsterAbilitySystem = Monster->GetMAbilitySystemComponent();
		UMWeaponItemDefinition* WeaponDefCDO = Cast<UMWeaponItemDefinition>(ItemDef);
		const UPMCombatSet* CombatSet = OwnerAbilitySystem->GetSet<UPMCombatSet>();
		const UPMCombatSet* MonsterCombatSet = MonsterAbilitySystem ? MonsterAbilitySystem->GetSet<UPMCombatSet>() : nullptr;

		if (OwnerAbilitySystem && WeaponDefCDO && CombatSet && MonsterCombatSet)
		{
			const FGameplayEffectContextHandle& ContextHandle = OwnerAbilitySystem->MakeGameplayEffectContext(OwnerAbilitySystem->GetOwner(), WeakWeaponActor.Get(), HitResult);

			TMap<FGameplayTag, float> SetbyCallerMap;
			SetbyCallerMap.Add(FPMGameplayTags::Get().Ability_Effect_SetByCaller_AttackPower, CombatSet->GetAttackPower());
			SetbyCallerMap.Add(FPMGameplayTags::Get().Ability_Effect_SetByCaller_DefensePower, MonsterCombatSet->GetDefensePower());

			FGameplayEffectSpec Spec = OwnerAbilitySystem->MakeGameplayEffectSpecWithSetByCaller(ContextHandle, WeaponDefCDO->DefaultAttackEffectClass, SetbyCallerMap, GameplayCue_Combat_Hit);

			OwnerAbilitySystem->ApplyGameplayEffectSpecToTarget(Spec, MonsterAbilitySystem);
		}

		OverlappedActors.Add(HitActor);
	}
}

void UMAbility_DefaultAttackBase::NotifyMontageEndCallBack()
{
	MontageIndex = 0;
	const FGameplayAbilityActivationInfo ActivationInfo = GetCurrentActivationInfo();
	if (HasAuthority(&ActivationInfo))
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), ActivationInfo, true, false);
	}
}

bool UMAbility_DefaultAttackBase::PlayMontage()
{
	UAnimMontage* Montage = ItemDef->DefaultAttackMontages.IsValidIndex(MontageIndex) ? ItemDef->DefaultAttackMontages[MontageIndex] : nullptr;
	if (!Montage)
	{
		return false;
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageAndWait = nullptr;
	PlayMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, FName(GetName()), Montage);

	MontageIndex = (MontageIndex + 1) % ItemDef->DefaultAttackMontages.Num();

	if (PlayMontageAndWait)
	{
		bCanCombo = false;

		PlayMontageAndWait->OnCompleted.AddDynamic(this, &UMAbility_DefaultAttackBase::NotifyMontageEndCallBack);
		PlayMontageAndWait->OnBlendOut.AddDynamic(this, &UMAbility_DefaultAttackBase::NotifyMontageEndCallBack);
		PlayMontageAndWait->OnCancelled.AddDynamic(this, &UMAbility_DefaultAttackBase::NotifyMontageEndCallBack);
		PlayMontageAndWait->OnInterrupted.AddDynamic(this, &UMAbility_DefaultAttackBase::NotifyMontageEndCallBack);

		PlayMontageAndWait->ReadyForActivation();
		return true;
	}

	return false;
}

bool UMAbility_DefaultAttackBase::BindAnimNotify()
{
	UAbilityTask_WaitGameplayEvent* WaitAttactStart =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, Animation_Notify_StartAttack);
	UAbilityTask_WaitGameplayEvent* WaitAttackEnd =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, Animation_Notify_EndAttack);
	if (WaitAttactStart && WaitAttackEnd)
	{
		WaitAttactStart->EventReceived.AddDynamic(this, &UMAbility_DefaultAttackBase::StartAttackTracing);
		WaitAttackEnd->EventReceived.AddDynamic(this, &UMAbility_DefaultAttackBase::EndAttackTracing);
		WaitAttactStart->ReadyForActivation();
		WaitAttackEnd->ReadyForActivation();

		return true;
	}

	return false;
}

bool UMAbility_DefaultAttackBase::RotateToTarget(AActor* Target)
{
	if (!Target)
	{
		return false;
	}

	UMAbilityTask_RotateToActor* Task = UMAbilityTask_RotateToActor::CreateRotateToActorTask(this, Target);
	if (Task)
	{
		Task->ReadyForActivation();
		return true;
	}

	return false;
}

AActor* UMAbility_DefaultAttackBase::FindNearlestTarget() const
{
	AMPlayerCharacterBase* Player = Cast<AMPlayerCharacterBase>(GetActorInfo().AvatarActor);
	AActor* Nearlest = nullptr;
	if (Player)
	{
		for (AActor* Actor : Player->GetOverlappedMonsters())
		{
			AMMonsterBase* Monster = Cast<AMMonsterBase>(Actor);
			if (Monster && Monster->GetCharacterLifeState() == EMCharacterLiftState::Alive)
			{
				if (!Nearlest)
				{
					Nearlest = Monster;
					continue;
				}

				float Dist1 = FVector::Distance(Monster->GetActorLocation(), Player->GetActorLocation());
				float Dist2 = FVector::Distance(Nearlest->GetActorLocation(), Player->GetActorLocation());
				if (Dist1 < Dist2)
				{
					Nearlest = Monster;
				}
			}
		}
	}

	return Nearlest;
}
