#include "PMPawnExtensionComponent.h"
#include "PMGameplayTags.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/Controller.h"
#include "AbilitySystem/PMAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "Character/MPlayerCharacterBase.h"

const FName UPMPawnExtensionComponent::NAME_ActorFeatureName{"PawnExtension"};

UPMPawnExtensionComponent::UPMPawnExtensionComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// 초기화 단계 검사를 틱으로 하지 않고 이벤트 발생 형태로 가기 때문에 틱을 끈다. 하위 클래스에서 필요할 때 켠다.
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UPMPawnExtensionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPMPawnExtensionComponent, PawnData);
}

void UPMPawnExtensionComponent::InitializeAbilitySystem(UPMAbilitySystemComponent* InAbilitySystemComponent, AActor* InOwnerActor)
{
	check(InAbilitySystemComponent && InOwnerActor);

	if (AbilitySystemComponent == InAbilitySystemComponent)
	{
		return;
	}

	if (AbilitySystemComponent)
	{
		UninitializeAbilitySystem();
	}

	APawn* Pawn = GetPawnChecked<APawn>();
	AActor* ExistingAvatar = InAbilitySystemComponent->GetAvatarActor();
	if ((ExistingAvatar != nullptr) && (ExistingAvatar != Pawn) && ExistingAvatar->IsA(APawn::StaticClass()))
	{
		ensure(!ExistingAvatar->HasAuthority());

		if (UPMPawnExtensionComponent* OtherExtensionComponent = FindPawnExtensionComponent(ExistingAvatar))
		{
			OtherExtensionComponent->UninitializeAbilitySystem();
		}
	}

	AbilitySystemComponent = InAbilitySystemComponent;
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Pawn);

	// 어빌리티 준비 완료됐다고 호출
	OnAbilitySystemInitialized.Broadcast();
}

void UPMPawnExtensionComponent::UninitializeAbilitySystem()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
	{
		if (AbilitySystemComponent->GetOwnerActor() != nullptr)
		{
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}
		else
		{
			AbilitySystemComponent->ClearActorInfo();
		}

		OnAbilitySystemUninitialized.Broadcast();
	}

	AbilitySystemComponent = nullptr;
}

void UPMPawnExtensionComponent::HandleControllerChanged()
{
	CheckDefaultInitialization();
}

void UPMPawnExtensionComponent::OnRegister()
{
	Super::OnRegister();

	if (!GetPawn<APawn>())
	{
		MCHAE_ERROR("this component has been added to a BP whoes base class is not a Pawn!");
		return;
	}

	// GameFeature시스템을 사용하고싶은 Component는 모두 RegisterInitStateFeature를 호출해야한다.
	// 그래야지 GameFrameworkComponentManager에서 해당 컴포넌트의 초기화 과정을 제어할 수 있다.
	// 내부적으로 이 컴포넌트를 기반으로 OwnerActor, this를 이용해 어떤 값을 주지 않아도 알아서 등록해서 컨트롤해준다.
	RegisterInitStateFeature();
}

void UPMPawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	// FeatureName이 None이라면 OwnerActor에 등록된 모든 Feature Component들의 상태를 관찰.
	BindOnActorInitStateChanged(
	NAME_None/*어떤 Feature Component의 상태를 관찰할것인가?*/,
	FGameplayTag{}/*어떤 상태로 변경될때 이벤트를 호출하고 싶은가? 지금은 모든 상태.*/,
	false);

	// 이 컴포넌트의 상태를 InitState_Spawned로 변경한다.
	// CanChageInitState(override) -> HandleChangeInitState(override) -> BindOnActorInitStateChanged(OnActorInitStateChanged호출) 의 순서로 진행된다.
	ensure(TryToChangeInitState(FPMGameplayTags::Get().InitState_Spawned));

	CheckDefaultInitialization();
}

void UPMPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//RegisterInitStateFeature와 쌍을 맞춰서 꼭 호출해줘야한다!
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void UPMPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	// 나 자신이 아닌( 하위 컴포넌트의 )상태가 변경되었으니 다시 검사 시작
	if (Params.FeatureName != NAME_ActorFeatureName)
	{
		const FPMGameplayTags& InitTags = FPMGameplayTags::Get();
		if (Params.FeatureState == InitTags.InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

bool UPMPawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();
	const FPMGameplayTags& InitTags = FPMGameplayTags::Get();

	// 처음 생성되고 난 후 Spawned상태로 넘어갈 때 Pawn이 존재하면 OK
	if (!CurrentState.IsValid() && DesiredState == InitTags.InitState_Spawned)
	{
		if (Pawn)
		{
			return true;
		}
	}

	// 현재 상태가 InitState_Spawned이고, InitState_DataAvailable로 넘어가고 싶을 때 Controller가 존재하면 OK
	if (CurrentState == InitTags.InitState_Spawned && DesiredState == InitTags.InitState_DataAvailable)
	{
		if (!PawnData)
		{
			return false;
		}

		const bool bHasAuthority = Pawn->HasAuthority();
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		if (bHasAuthority || bIsLocallyControlled)
		{
			if (!GetController<AController>())
			{
				return false;
			}
		}

		return true;
	}

	// InitState_DataAvailable->InitState_DataInitialized일 때
	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		// 모든 GameFeature가 InitState_DataAvailable일 때 OK
		// 만약 하나라도 InitState_DataAvailable가 아니면 대기했다가 해당 컴포넌트가 상태가 변경됐을 때 OnActorInitStateChanged가 호출되어 다시 시작
		return Manager->HaveAllFeaturesReachedInitState(Pawn, InitTags.InitState_DataAvailable);
	}

	// InitState_DataInitialized->InitState_GameplayReady일 때
	if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void UPMPawnExtensionComponent::CheckDefaultInitialization()
{
	// 나를 제외하고 Actor에 부착된 모든 GameFeature Component들에 대해 CheckDefaultInitialization를 실행해준다.
	CheckDefaultInitializationForImplementers();

	const FPMGameplayTags& InitTags = FPMGameplayTags::Get();
	static const TArray<FGameplayTag> StateChain = { InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized, InitTags.InitState_GameplayReady };

	// 초기화 과정을 등록하고, 초기화가 가능한 시점까지 진행해준다.
	// ex) InitState_Spawned(ok) -> InitState_DataAvailable(ok) -> InitState_DataInitialized(fail) -> end
	ContinueInitStateChain(StateChain);
}

void UPMPawnExtensionComponent::SetupPlayerInputComponent()
{
	// 컨트롤러가 초기화가 완료되었을 때 다시한번 시작하기 위함
	CheckDefaultInitialization();
}

void UPMPawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemInitialized.Add(Delegate);
	}

	if (AbilitySystemComponent)
	{
		Delegate.Execute();
	}
}

void UPMPawnExtensionComponent::OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnAbilitySystemUninitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemUninitialized.Add(Delegate);
	}
}

UPMAbilitySystemComponent* UPMPawnExtensionComponent::GetPMAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

void UPMPawnExtensionComponent::SetPawnData(const UPMPawnData* InPawnData)
{
	APawn* Pawn = GetPawnChecked<APawn>();
	if (Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		return;
	}

	PawnData = InPawnData;

	Pawn->ForceNetUpdate();

	OnRep_PawnData();
}

void UPMPawnExtensionComponent::OnRep_PawnData()
{
	CheckDefaultInitialization();
}
