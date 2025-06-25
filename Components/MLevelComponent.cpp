#include "MLevelComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/PMPlayerControllerBase.h"
#include "AbilitySystem/PMAbilitySystemComponent.h"
#include "PMGameplayTags.h"
#include "System/MDataTableManager.h"
#include "GameFramework/Character.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "AbilitySystem/MGameplayEffectSet.h"

UMLevelComponent::UMLevelComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
}

void UMLevelComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMLevelComponent, CurrentLevel);
	DOREPLIFETIME_CONDITION(UMLevelComponent, CurrentExperiencePoint, COND_OwnerOnly);
}

void UMLevelComponent::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystemComp = GetAbilitySystemComponent<UPMAbilitySystemComponent>();
	if (!AbilitySystemComp)
	{
		MCHAE_ERROR("AbilitySystem is not initialized!!! Level system requires gameplay event!!");
		check(false);
	}
	if (HasAuthority())
	{
		AbilitySystemComp->AddGameplayEventTagContainerDelegate(
			FGameplayTagContainer(FPMGameplayTags::Get().GameplayEvent_Character_ChangeExp),
			FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UMLevelComponent::OnChange_ExperiencePoint));
	}

	InitLevel();
}

FDelegateHandle UMLevelComponent::CallAndRegister_OnChangeLevel(FOnChange_Level::FDelegate&& Delegate)
{
	if (AbilitySystemComp)
	{
		Delegate.ExecuteIfBound(CurrentLevel - 1, CurrentLevel, CurrentMaxExperiencePoint);
	}

	return Delegate_OnChangeLevel.Add(MoveTemp(Delegate));
}

FDelegateHandle UMLevelComponent::CallAndRegister_OnChangeExp(FOnChange_Exp::FDelegate&& Delegate)
{
	if (AbilitySystemComp)
	{
		Delegate.ExecuteIfBound(0, CurrentExperiencePoint);
	}

	return Delegate_OnChangeExp.Add(MoveTemp(Delegate));
}

void UMLevelComponent::RemoveDelegate_OnChangeLevel(const FDelegateHandle& Handle)
{
	Delegate_OnChangeLevel.Remove(Handle);
}

void UMLevelComponent::RemoveDelegate_OnChangeExp(const FDelegateHandle& Handle)
{
	Delegate_OnChangeExp.Remove(Handle);
}

void UMLevelComponent::InitLevel()
{
	SetMaxExperiencePoint(CurrentLevel);
	BroadcastOnChangeLevel(CurrentLevel);
	BroadcastOnChangeExp(0);
}

void UMLevelComponent::SetMaxExperiencePoint(const int32 Level)
{
	int32 RowId = UMDataTableManager::MakeRowId((int32)EMTableKey::LevelTable, Level);
	CurrentLevelTableRow = UMDataTableManager::GetTableRowData<FMTable_LevelTable>(this, RowId);
	if (CurrentLevelTableRow)
	{
		CurrentMaxExperiencePoint = CurrentLevelTableRow->MaxExperiencePoint;
	}
}

void UMLevelComponent::OnChange_ExperiencePoint(FGameplayTag Tag, const FGameplayEventData* EventData)
{
	if (AbilitySystemComp && EventData)
	{
		CurrentExperiencePoint += EventData->EventMagnitude;
		if (GetNetMode() == ENetMode::NM_Standalone)
		{
			OnRep_OnChangeExperience(CurrentExperiencePoint - EventData->EventMagnitude);
		}
		if (CurrentExperiencePoint >= CurrentMaxExperiencePoint)
		{
			LevelUp();
		}
	}
}

void UMLevelComponent::LevelUp()
{
	CurrentLevel++;
	int64 OldExp = CurrentExperiencePoint;
	int64 NewExp = CurrentExperiencePoint - CurrentMaxExperiencePoint;
	CurrentExperiencePoint = NewExp < 0 ? 0 : NewExp;

	ApplyLevelUpStat();

	if (GetNetMode() == ENetMode::NM_Standalone)
	{
		OnRep_OnChangeLevel(CurrentLevel - 1);
	}
	else
	{
		// Server broadcasting
		SetMaxExperiencePoint(CurrentLevel);
		BroadcastOnChangeLevel(CurrentLevel - 1);
	}

	if (GetNetMode() == ENetMode::NM_Standalone)
	{
		OnRep_OnChangeExperience(OldExp);
	}
	else
	{
		// Server broadcasting
		BroadcastOnChangeExp(OldExp);
	}

	if (CurrentExperiencePoint >= CurrentMaxExperiencePoint)
	{
		LevelUp();
	}
}

void UMLevelComponent::ApplyLevelUpStat()
{
	if (CurrentLevelTableRow)
	{
		if (AppliedEffectHandles.WasApplied())
		{
			AppliedEffectHandles.RemoveAppliedEffects(AbilitySystemComp);
		}

		UMGameplayEffectSet* EffectSet = CurrentLevelTableRow->EffectSet.LoadSynchronous();
		if (EffectSet && AbilitySystemComp)
		{
			auto Callback = [this](const FGameplayTag& StatTag, const float DefaultValue)->float
			{
				return CalculateAdditiveStatByLevel(StatTag, DefaultValue);
			};
			EffectSet->ApplyGameplayEffectsToAsc(AbilitySystemComp, &AppliedEffectHandles, Callback, AbilitySystemComp->GetOwner(), nullptr, this);
		}
	}
}

float UMLevelComponent::CalculateAdditiveStatByLevel(const FGameplayTag& StatTag, const float DefaultValue)
{
	if (StatTag == FPMGameplayTags::Get().Ability_Effect_SetByCaller_MaxHealth)
	{
		return (CurrentLevel - 1) * DefaultValue + CurrentLevel * 1.1;
	}
	else if (StatTag == FPMGameplayTags::Get().Ability_Effect_SetByCaller_Health)
	{
		return (CurrentLevel - 1) * DefaultValue + CurrentLevel * 1.1;
	}
	else if (StatTag == FPMGameplayTags::Get().Ability_Effect_SetByCaller_AttackPower)
	{
		return (CurrentLevel - 1) * DefaultValue + FMath::Floor(CurrentLevel / 5);
	}
	else if (StatTag == FPMGameplayTags::Get().Ability_Effect_SetByCaller_DefensePower)
	{
		return (CurrentLevel - 1) * DefaultValue;
	}
	else if (StatTag == FPMGameplayTags::Get().Ability_Effect_SetByCaller_HealthRecovery)
	{
		return (CurrentLevel - 1) * DefaultValue;
	}

	return 0.0f;
}

void UMLevelComponent::OnRep_OnChangeLevel(const int32 OldLevel)
{
	if (OldLevel < CurrentLevel)
	{
		ACharacter* Character = GetPawn<ACharacter>();
		if (Character && CurrentLevelTableRow)
		{
			if (CurrentLevelTableRow->LevelUpParticle)
			{
				UNiagaraFunctionLibrary::SpawnSystemAttached(
					CurrentLevelTableRow->LevelUpParticle,
					Character->GetMesh(),
					FName("root"),
					FVector::ZeroVector,
					FRotator::ZeroRotator,
					FVector(.5f),
					EAttachLocation::Type::SnapToTargetIncludingScale,
					true,
					ENCPoolMethod::AutoRelease
				);
			}

			if (CurrentLevelTableRow->LevelUpSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, CurrentLevelTableRow->LevelUpSound, Character->GetActorLocation());
			}
		}
	}

	SetMaxExperiencePoint(CurrentLevel);
	BroadcastOnChangeLevel(OldLevel);
}

void UMLevelComponent::OnRep_OnChangeExperience(const int64 OldExperiencePoint)
{
	BroadcastOnChangeExp(OldExperiencePoint);
}

void UMLevelComponent::BroadcastOnChangeLevel(const int32 OldLevel)
{
	if (Delegate_OnChangeLevel.IsBound())
	{
		Delegate_OnChangeLevel.Broadcast(OldLevel, CurrentLevel, CurrentMaxExperiencePoint);
	}
}

void UMLevelComponent::BroadcastOnChangeExp(const int64 OldExperiencePoint)
{
	if (Delegate_OnChangeExp.IsBound())
	{
		Delegate_OnChangeExp.Broadcast(OldExperiencePoint, CurrentExperiencePoint);
	}
}

void UMLevelComponent::Debug_LevelUp()
{
	Debug_LevelUpServer();
}

void UMLevelComponent::Debug_LevelUpServer_Implementation()
{
	LevelUp();
}
