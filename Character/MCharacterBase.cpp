#include "MCharacterBase.h"
#include "Components/MNameWidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/MCharacterMovementComponent.h"

AMCharacterBase::AMCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UMCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	NameComponent = CreateDefaultSubobject<UMNameWidgetComponent>(TEXT("NameComp"));
	NameComponent->SetupAttachment(GetRootComponent());

	bReplicates = true;
}

void AMCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMCharacterBase, CharacterLifeState);
	DOREPLIFETIME(AMCharacterBase, CharacterStateFlag);
}

void AMCharacterBase::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AMCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AMCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	InitCharacterName();
}

bool AMCharacterBase::IsMoveInputIgnored() const
{
	if (IsOnCharacterStateFlags(EMCharacterStateFlag::BlockMovement) || CharacterLifeState >= EMCharacterLiftState::ReadyToDead)
	{
		return false;
	}

	return Super::IsMoveInputIgnored();
}

void AMCharacterBase::InitCharacterName()
{
	NameComponent->InitNameWidget();
}

void AMCharacterBase::OnDead()
{
	if (HasAuthority())
	{
		Server_AddCharacterStateFlag(EMCharacterStateFlag::BlockAll);
	}
}

const FName& AMCharacterBase::GetCharacterName() const
{
	return CharacterName;
}

void AMCharacterBase::Server_SetCharacterLifeState_Implementation(const EMCharacterLiftState InState)
{
	if (InState > CharacterLifeState)
	{
		CharacterLifeState = InState;
	}
}

void AMCharacterBase::Server_AddCharacterStateFlag_Implementation(const int64& InState)
{
	const int64& OldFlag = CharacterStateFlag;
	CharacterStateFlag |= InState;

	if (Delegate_OnChangeCharacterStateFlags.IsBound())
	{
		Delegate_OnChangeCharacterStateFlags.Broadcast(OldFlag, CharacterStateFlag);
	}
}

void AMCharacterBase::Server_RemoveCharacterStateFlag_Implementation(const int64& InState)
{
	const int64& OldFlag = CharacterStateFlag;
	CharacterStateFlag &= ~InState;

	if (Delegate_OnChangeCharacterStateFlags.IsBound())
	{
		Delegate_OnChangeCharacterStateFlags.Broadcast(OldFlag, CharacterStateFlag);
	}
}

void AMCharacterBase::Multicast_PlayMontage_Implementation(UAnimMontage* MontageToPlay, float InPlayRate /*= 1.f*/, EMontagePlayReturnType ReturnValueType /*= EMontagePlayReturnType::MontageLength*/, float InTimeToStartMontageAt /*= 0.f*/, bool bStopAllMontages /*= true*/)
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	UAnimInstance* AnimInstance = MeshComp ? MeshComp->GetAnimInstance() : nullptr;
	if (MontageToPlay && AnimInstance)
	{
		AnimInstance->Montage_Play(MontageToPlay);
	}
}

void AMCharacterBase::OnRep_OnChangeStateFlags(const int64& OldFlag)
{
	if (Delegate_OnChangeCharacterStateFlags.IsBound())
	{
		Delegate_OnChangeCharacterStateFlags.Broadcast(OldFlag, CharacterStateFlag);
	}
}
