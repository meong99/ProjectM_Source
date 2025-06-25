#include "MPlayerCharacterBase.h"
#include "Components/PMPawnExtensionComponent.h"
#include "Camera/PMCameraComponent.h"
#include "AbilitySystem/PMAbilitySystemComponent.h"
#include "Components/PMHealthComponent.h"
#include "Input/PMInputComponent.h"
#include "UI/MViewportClient.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/MCharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Util/MGameplayStatics.h"
#include "Components/MNavigationComponent.h"
#include "Components/PMCharacterInitComponent.h"
#include "Character/PMPawnData.h"
#include "PMGameplayTags.h"
#include "Player/PMPlayerState.h"
#include "GameplayEffect.h"
#include "Types/MTeamTypes.h"
#include "Components/MPawnComponentBase.h"
#include "AbilitySystem/MGameplayEffectSet.h"
#include "GameFramework/PlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"

AMPlayerCharacterBase::AMPlayerCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UMCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// 초기화 단계 검사를 틱으로 하지 않고 이벤트 발생 형태로 가기 때문에 틱을 끈다. 하위 클래스에서 필요할 때 켠다.
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = false;

	OverrideInputComponentClass = UPMInputComponent::StaticClass();

	GetCapsuleComponent()->SetCollisionProfileName(*UEnum::GetDisplayValueAsText(EMCollisionChannel::Player).ToString());

	PawnExtComp	= CreateDefaultSubobject<UPMPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	PawnExtComp->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &AMPlayerCharacterBase::Callback_OnAbilitySystemInitialized));
	PawnExtComp->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &AMPlayerCharacterBase::Callback_OnAbilitySystemUninitialzed));

	CameraComponent = CreateDefaultSubobject<UPMCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetRelativeLocation(FVector(-300.f, 0.f, 75.f));

	HealthComponent = CreateDefaultSubobject<UPMHealthComponent>(TEXT("HealthComponent"));
	
	NetCullDistanceSquared = 900000000.0f;
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickMontagesAndRefreshBonesWhenPlayingMontages;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	NavigationComponent = CreateDefaultSubobject<UMNavigationComponent>(TEXT("NavigationComponent"));
}

void AMPlayerCharacterBase::OnDead()
{
	if (CharacterLifeState > EMCharacterLiftState::Alive)
	{
		return;
	}

	if (HasAuthority())
	{
		Super::OnDead();
		CharacterLifeState = EMCharacterLiftState::Dead;

		const UPMPawnData* PawnData = PawnExtComp->GetPawnData();
		Multicast_PlayMontage(PawnData->DeathAnimation);

		UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
		if (AbilitySystemComponent)
		{
			for (AActor* Monster : OverlappedMonsters)
			{
				FGameplayEventData EventData;
				EventData.Target = Monster;
				EventData.Instigator = AbilitySystemComponent->GetOwner();
				EventData.EventTag = FPMGameplayTags::Get().GameplayEvent_Character_State_Dead;
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Monster, FPMGameplayTags::Get().GameplayEvent_Character_State_Dead, EventData);
			}
		}
	}

	if (GetNetMode() != NM_DedicatedServer)
	{
		UMViewportClient* ViewportClient = UMViewportClient::Get(this);
		if (IsValid(ViewportClient))
		{
			FMWidgetInfo Info{ this, this };
			ViewportClient->AddWidgetToLayer(FPMGameplayTags::Get().UI_Registry_Game_Restart, Info);
		}
	}
	SetGenericTeamId(FGenericTeamId::NoTeam);
}

void AMPlayerCharacterBase::Restart()
{
	Super::Restart();
	if (HasAuthority())
	{
		CharacterLifeState = EMCharacterLiftState::Alive;
		Server_RemoveCharacterStateFlag(EMCharacterStateFlag::BlockAll);
	}
	else
	{
		USkeletalMeshComponent* MeshComp = GetMesh();
		UAnimInstance* AnimInstance = MeshComp ? MeshComp->GetAnimInstance() : nullptr;
		if (AnimInstance)
		{
			AnimInstance->StopAllMontages(false);
		}
	}
	SetGenericTeamId((int32)EMGenericTeamId::Player);
}

void AMPlayerCharacterBase::PawnClientRestart()
{
	Super::PawnClientRestart();

	const TSet<UActorComponent*>& SubComponents = GetComponents();
	for (UActorComponent* Component : SubComponents)
	{
		UMPawnComponentBase* SubComponent = Cast<UMPawnComponentBase>(Component);
		if (SubComponent)
		{
			SubComponent->PawnClientRestart();
		}
	}
	SetGenericTeamId((int32)EMGenericTeamId::Player);
}

void AMPlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AMPlayerCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitCharacterName();
}

void AMPlayerCharacterBase::InitCharacterName()
{
	APMPlayerState* MyPlayerState = Cast<APMPlayerState>(GetPlayerState());
	
	if (MyPlayerState)
	{
		CharacterName = *MyPlayerState->GetPlayerName();
	}

	Super::InitCharacterName();
}

void AMPlayerCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Dedi에서도 Controller가 존재할 때 초기화 단계를 다시 시작해주는 부분이 필요하다.
	// Client는 SetupPlayerInputComponent에서 실행
	if (GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		PawnExtComp->HandleControllerChanged();
	}

	const TSet<UActorComponent*>& SubComponents = GetComponents();
	for (UActorComponent* Component : SubComponents)
	{
		UMPawnComponentBase* SubComponent = Cast<UMPawnComponentBase>(Component);
		if (SubComponent)
		{
			SubComponent->PossessedBy(NewController);
		}
	}
}

void AMPlayerCharacterBase::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// GameFeature를 통한 초기화를 하고싶은 컴포넌트를 위한 초기화 시작 단계
	PawnExtComp->SetupPlayerInputComponent();

	UPMCharacterInitComponent* CharacterInitComp = FindComponentByClass<UPMCharacterInitComponent>();
	if (InputComponent != nullptr && CharacterInitComp)
	{
		CharacterInitComp->InitializePlayerInput(InputComponent);
	}

	if (OnSetInputComponentDelegate.IsBound())
	{
		OnSetInputComponentDelegate.Broadcast(InputComponent);
		OnSetInputComponentDelegate.Clear();
	}
}

UAbilitySystemComponent* AMPlayerCharacterBase::GetAbilitySystemComponent() const
{
	return PawnExtComp->GetPMAbilitySystemComponent();
}

UPMAbilitySystemComponent* AMPlayerCharacterBase::GetMAbilitySystemComponent() const
{
	return Cast<UPMAbilitySystemComponent>(GetAbilitySystemComponent());
}

void AMPlayerCharacterBase::Callback_OnAbilitySystemInitialized()
{
	UPMAbilitySystemComponent* ASC = Cast<UPMAbilitySystemComponent>(GetAbilitySystemComponent());
	check(ASC);

	HealthComponent->InitializeWithAbilitySystem(ASC);
}

void AMPlayerCharacterBase::Callback_OnAbilitySystemUninitialzed()
{
	HealthComponent->UninitializeWithAbilitySystem();
}

void AMPlayerCharacterBase::CallOrRegister_OnSetInputComponent(FOnSetInputComponent::FDelegate&& Delegate)
{
	if (IsValid(InputComponent))
	{
		Delegate.ExecuteIfBound(InputComponent);
	}
	else
	{
		OnSetInputComponentDelegate.Add(MoveTemp(Delegate));
	}
}

void AMPlayerCharacterBase::AddOverlappedMonster(AActor* Monster)
{
	if (Monster)
	{
		OverlappedMonsters.Add(Monster);
	}
}

void AMPlayerCharacterBase::RemoveOverlappedMonster(AActor* Monster)
{
	if (Monster)
	{
		OverlappedMonsters.Remove(Monster);
	}
}

void AMPlayerCharacterBase::InitCharacterDefaultSpec()
{
	UPMAbilitySystemComponent* AbilitySystemComponent = GetMAbilitySystemComponent();
	if (AbilitySystemComponent && HasAuthority())
	{
		APMPlayerState* CurPlayerState = GetPlayerState<APMPlayerState>();
		const UPMPawnData* PawnData = CurPlayerState ? CurPlayerState->GetPawnData() : nullptr;
		if (PawnData)
		{
			for (const UMGameplayEffectSet* EffectSet : PawnData->EffectSets)
			{
				if (EffectSet)
				{
					FMAbilitySet_AppliedEffectHandles TempHandles;
					EffectSet->ApplyGameplayEffectsToAsc(AbilitySystemComponent, &TempHandles, AbilitySystemComponent->GetOwner(), this, nullptr);
				}
			}
		}
		else
		{
			ensure(false);
			MCHAE_ERROR("Can't access data for initialize character default spec!!!!");
		}
	}
}

void AMPlayerCharacterBase::ClearAbilityActorInfo()
{
	UPMAbilitySystemComponent* AbilitySystemComponent = GetMAbilitySystemComponent();
	if (AbilitySystemComponent && AbilitySystemComponent->AbilityActorInfo.IsValid())
	{
		AbilitySystemComponent->ClearActorInfo();
	}
}

void AMPlayerCharacterBase::InitAbilityActorInfo()
{
	UPMAbilitySystemComponent* AbilitySystemComponent = GetMAbilitySystemComponent();
	if (AbilitySystemComponent && !AbilitySystemComponent->AbilityActorInfo.IsValid())
	{
		AbilitySystemComponent->InitAbilityActorInfo(GetPlayerState(), this);
	}
}

#if WITH_EDITOR
void AMPlayerCharacterBase::Test_KillPlayer()
{
	if (GetNetMode() == NM_Client)
	{
		OnDead();
	}
	Server_KillPlayer();
}

void AMPlayerCharacterBase::Server_KillPlayer_Implementation()
{
	OnDead();
}

#endif
