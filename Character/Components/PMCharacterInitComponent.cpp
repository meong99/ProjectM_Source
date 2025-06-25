#include "PMCharacterInitComponent.h"
#include "PMPawnExtensionComponent.h"
#include "PMGameplayTags.h"
#include "Components/GameFrameworkComponentDelegates.h"
#include "Player/PMPlayerState.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Player/PMPlayerControllerBase.h"
#include "Character/PMPawnData.h"
#include "Camera/PMCameraComponent.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "Input/PMInputComponent.h"
#include "InputActionValue.h"
#include "AbilitySystem/PMAbilitySystemComponent.h"
#include "UI/MViewportClient.h"
#include "Engine/GameInstance.h"
#include "GameFramework/Character.h"
#include "Components/MNavigationComponent.h"

const FName UPMCharacterInitComponent::NAME_ActorFeatureName{"CharacterInit"};
const FName UPMCharacterInitComponent::NAME_BindInputsNow{"BindInputsNow"};

UPMCharacterInitComponent::UPMCharacterInitComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// 초기화 단계 검사를 틱으로 하지 않고 이벤트 발생 형태로 가기 때문에 틱을 끈다. 하위 클래스에서 필요할 때 켠다.
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void UPMCharacterInitComponent::OnRegister()
{
	Super::OnRegister();

	if (!GetPawn<APawn>())
	{
		MCHAE_ERROR("this component has been added to a BP whoes base class is not a Pawn!");
		return;
	}

	RegisterInitStateFeature();
}

void UPMCharacterInitComponent::BeginPlay()
{
	Super::BeginPlay();

	// PawnExtensionComponent의 상태 변화를 관찰한다.
	BindOnActorInitStateChanged(
		UPMPawnExtensionComponent::NAME_ActorFeatureName,
		FGameplayTag{},
		false);

	// 이 컴포넌트의 상태를 InitState_Spawned로 변경한다.
	// CanChageInitState(override) -> HandleChangeInitState(override) -> BindOnActorInitStateChanged(OnActorInitStateChanged호출) 의 순서로 진행된다.
	ensure(TryToChangeInitState(FPMGameplayTags::Get().InitState_Spawned));

	CheckDefaultInitialization();
}

void UPMCharacterInitComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void UPMCharacterInitComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	const FPMGameplayTags& InitTags = FPMGameplayTags::Get();

	// UPMPawnExtensionComponent의 상태가 변경되었으니 다음 초기화 단계로 넘어간다
	if (Params.FeatureName == UPMPawnExtensionComponent::NAME_ActorFeatureName)
	{
		// Data가 초기화 가능한 상태일 때 초기화 진행
		if (Params.FeatureState == InitTags.InitState_DataInitialized)
		{
			CheckDefaultInitialization();
		}
	}
}

bool UPMCharacterInitComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	const FPMGameplayTags& InitTags = FPMGameplayTags::Get();
	APawn* Pawn = GetPawn<APawn>();
	APMPlayerState* PlayerState = GetPlayerState<APMPlayerState>();

	if (!CurrentState.IsValid() && DesiredState == InitTags.InitState_Spawned)
	{
		if (Pawn)
		{
			return true;
		}
	}

	if (CurrentState == InitTags.InitState_Spawned && DesiredState == InitTags.InitState_DataAvailable)
	{
		if (!PlayerState)
		{
			return false;
		}

		return true;
	}

	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		// UPMPawnExtensionComponent가 InitState_DataInitialized일 때 OK
		return PlayerState && Manager->HasFeatureReachedInitState(Pawn, UPMPawnExtensionComponent::NAME_ActorFeatureName, InitTags.InitState_DataInitialized);
	}

	if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void UPMCharacterInitComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	const FPMGameplayTags& InitTags = FPMGameplayTags::Get();

	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		APMPlayerState* PlayerState = GetPlayerState<APMPlayerState>();

		if (!ensure(Pawn && PlayerState))
		{
			return;
		}

		// Pawn Setting
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const UPMPawnData* PawnData = nullptr;
		UPMPawnExtensionComponent* PawnExtComp = UPMPawnExtensionComponent::FindPawnExtensionComponent(Pawn);
		if (PawnExtComp)
		{
			PawnData = PawnExtComp->GetPawnData<UPMPawnData>();

			// AbilitySystemSetting
			PawnExtComp->InitializeAbilitySystem(PlayerState->GetPMAbilitySystemComponent(), PlayerState);
		}

		// Camera Setting
		if (bIsLocallyControlled && PawnData)
		{
			if (UPMCameraComponent* CameraComponent = UPMCameraComponent::FindCameraComponent(Pawn))
			{
				// CameraComponent의 GetCameraView는 컨트롤러에 의해 매 틱 호출되며
				// 아래 델리게이트에 바인딩을 하면 바인딩된 함수(DetermineCameraMode)를 통해 현재 Character의 CameraMode를 반환하도록 설정되어있다.
				CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &UPMCharacterInitComponent::DetermineCameraMode);
			}
		}
	}
}

void UPMCharacterInitComponent::CheckDefaultInitialization()
{
	const FPMGameplayTags& InitTags = FPMGameplayTags::Get();

	static const TArray<FGameplayTag> StateChain = { InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized, InitTags.InitState_GameplayReady };
	ContinueInitStateChain(StateChain);
}

TSubclassOf<UPMCameraMode> UPMCharacterInitComponent::DetermineCameraMode() const
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		MCHAE_WARNING("Pawn is not valid!");
		return nullptr;
	}

	if (const UPMPawnExtensionComponent* PawnExtComp = UPMPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UPMPawnData* PawnData = PawnExtComp->GetPawnData<UPMPawnData>())
		{
			return PawnData->GetDefaultCameraMode();
		}
	}

	MCHAE_WARNING("Can't get CameraMode. check this function.");

	return nullptr;
}

void UPMCharacterInitComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PlayerController = GetController<APlayerController>();
	check(PlayerController);

	const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	check(LocalPlayer);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	// 우선 초기화
	//Subsystem->ClearAllMappings();

	// PawnData에 입력한 Input값을 가져와서 설정한다.
	if (const UPMPawnExtensionComponent* PawnExtComp = UPMPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UPMPawnData* PawnData = PawnExtComp->GetPawnData<UPMPawnData>())
		{
			if (const UPMInputConfig* InputConfig = PawnData->GetInputConfig())
			{
				const FPMGameplayTags& GameplayTags = FPMGameplayTags::Get();

				for (const FPMMappableConfigPair& Pair : DefaultInputConfigs)
				{
					if (Pair.bShouldActivateAutomatically)
					{
						FModifyContextOptions Options = {};
						Options.bIgnoreAllPressedKeysUntilRelease = false;

						Subsystem->AddPlayerMappableConfig(Pair.Config.LoadSynchronous(), Options);
					}
				}

				// 태그로 구분지어서 입력 바인딩
				InputComponent = CastChecked<UPMInputComponent>(PlayerInputComponent);

				// 기본 입력 관련 Action 바인딩 (복잡한 Value가진 값 Vector... 등)
				InputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, false);
				InputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, false);
				InputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Jump, ETriggerEvent::Triggered, this, &ThisClass::Input_Jump, false);

				// 어빌리티와 관련된 InputAction 바인딩
				TArray<uint32> BindHandles;
				InputComponent->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, BindHandles);

				// Boolean Toogle입력을 받을 액션 바인딩
				InputComponent->BindToggleActions(InputConfig, this, &ThisClass::Input_ToggleInputTag, BindHandles);
			}
		}
	}

	// GameFeatureAction_AddInputConfig의 HandlePawnExtion을 호출.
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

void UPMCharacterInitComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	AMCharacterBase* CharacterBase = GetPawn<AMCharacterBase>();
	AController* Controller = CharacterBase ? CharacterBase->GetController() : nullptr;

	if (Controller && !CharacterBase->IsOnCharacterStateFlags(EMCharacterStateFlag::BlockMovement))
	{
		if (CharacterBase->IsOnCharacterStateFlags(EMCharacterStateFlag::ControlledFromNavigation))
		{
			UMNavigationComponent* NavComp = CharacterBase->FindComponentByClass<UMNavigationComponent>();
			if (NavComp)
			{
				NavComp->Server_DeactivateNavigation();
			}
		}
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			CharacterBase->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			CharacterBase->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void UPMCharacterInitComponent::Input_Jump(const FInputActionValue& InputActionValue)
{
	AMCharacterBase* Character = GetPawn<AMCharacterBase>();
	if (Character && Character->CanJump() && !Character->IsOnCharacterStateFlags(EMCharacterStateFlag::BlockMovement))
	{
		Character->Jump();
	}
}

void UPMCharacterInitComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();
	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		double AimInversionValue = -Value.Y;
		Pawn->AddControllerPitchInput(AimInversionValue);
	}
}

void UPMCharacterInitComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (AMCharacterBase* Character = GetPawn<AMCharacterBase>())
	{
		if (!Character->IsOnCharacterStateFlags(EMCharacterStateFlag::BlockAll))
		{
			if (const UPMPawnExtensionComponent* PawnExtComp = UPMPawnExtensionComponent::FindPawnExtensionComponent(Character))
			{
				if (UPMAbilitySystemComponent* ASC = PawnExtComp->GetPMAbilitySystemComponent())
				{
					ASC->AbilityInputTagPressed(InputTag);
				}
			}
		}
	}
}

void UPMCharacterInitComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (AMCharacterBase* Character = GetPawn<AMCharacterBase>())
	{
		if (!Character->IsOnCharacterStateFlags(EMCharacterStateFlag::BlockAll))
		{
			if (const UPMPawnExtensionComponent* PawnExtComp = UPMPawnExtensionComponent::FindPawnExtensionComponent(Character))
			{
				if (UPMAbilitySystemComponent* ASC = PawnExtComp->GetPMAbilitySystemComponent())
				{
					ASC->AbilityInputTagReleased(InputTag);
				}
			}
		}
	}
}

void UPMCharacterInitComponent::Input_ToggleInputTag(FGameplayTag InputTag)
{
	AMCharacterBase* Character = GetPawn<AMCharacterBase>();
	if (Character && InputComponent)
	{
		FInputActionDelegate::FDelegate* Delegate = InputComponent->Delegate_InputActionMap.Find(InputTag);
		if (Delegate && Delegate->IsBound())
		{
			Delegate->Execute(InputTag);
		}
	}
	else
	{
		MCHAE_ERROR("InputComponent is null!");
	}
}
