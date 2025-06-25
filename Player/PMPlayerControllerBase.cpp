#include "PMPlayerControllerBase.h"
#include "Camera/PMPlayerCameraManager.h"
#include "PMPlayerState.h"
#include "AbilitySystem/PMAbilitySystemComponent.h"
#include "Engine/GameInstance.h"
#include "UI/MViewportClient.h"
#include "Inventory/PMInventoryManagerComponent.h"
#include "Components/MPlayerTradeComponent.h"
#include "Components/MPlayerQuestComponent.h"
#include "Util/MGameplayStatics.h"
#include "UI/Level/MLoadingWidget.h"
#include "PMGameplayTags.h"

APMPlayerControllerBase::APMPlayerControllerBase()
{
	PlayerCameraManagerClass = APMPlayerCameraManager::StaticClass();
	InventoryManagerComponent = CreateDefaultSubobject<UPMInventoryManagerComponent>(TEXT("InventoryManagerComponent"));
	PlayerTradeComponent = CreateDefaultSubobject<UMPlayerTradeComponent>(TEXT("PlayerTradeComponent"));
	PlayerQuestComponent = CreateDefaultSubobject<UMPlayerQuestComponent>(TEXT("PlayerQuestComponent"));
	bReplicates = true;
}

void APMPlayerControllerBase::OnPossess(APawn* aPawn)
{
	if (IsValid(OldPawn))
	{
		OldPawn->Destroy();
	}
	OldPawn = GetPawn();

	Super::OnPossess(aPawn);

	AMPlayerCharacterBase* MyCharacter = Cast<AMPlayerCharacterBase>(GetPawn());
	if (MyCharacter)
	{
		Delegate_OnPossessed.Broadcast(MyCharacter);
	}

	const TSet<UActorComponent*>& SubComponents = GetComponents();
	for (UActorComponent* Component : SubComponents)
	{
		UMControllerComponentBase* SubComponent = Cast<UMControllerComponentBase>(Component);
		if (SubComponent)
		{
			SubComponent->OnPossess(aPawn);
		}
	}
}

void APMPlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();

	CallOrRegister_OnExperienceLoaded(FOnExperienceLoaded::FDelegate::CreateUObject(this, &APMPlayerControllerBase::OnExperienceLoaded));
	UMGameplayStatics::FadeIn(this, 1.5f, 1.f);
}

void APMPlayerControllerBase::BeginPlayingState()
{
	Super::BeginPlayingState();

	Delegate_OnReadyPlayerState.Broadcast(PlayerState);
	Delegate_OnReadyPlayerState.Clear();
}

void APMPlayerControllerBase::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UPMAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void APMPlayerControllerBase::ServerRestartPlayer_Implementation()
{
	Super::ServerRestartPlayer_Implementation();

	const TSet<UActorComponent*>& SubComponents = GetComponents();
	for (UActorComponent* Component : SubComponents)
	{
		UMControllerComponentBase* SubComponent = Cast<UMControllerComponentBase>(Component);
		if (SubComponent)
		{
			SubComponent->OnServerRestartPlayer();
		}
	}
}

void APMPlayerControllerBase::ClientRestart_Implementation(APawn* NewPawn)
{
	Super::ClientRestart_Implementation(NewPawn);

	const TSet<UActorComponent*>& SubComponents = GetComponents();
	for (UActorComponent* Component : SubComponents)
	{
		UMControllerComponentBase* SubComponent = Cast<UMControllerComponentBase>(Component);
		if (SubComponent)
		{
			SubComponent->OnClientRestart(NewPawn);
		}
	}
}

void APMPlayerControllerBase::BeginDestroy()
{
	Super::BeginDestroy();

	if (IsValid(OldPawn))
	{
		OldPawn->Destroy();
		OldPawn = nullptr;
	}
}

void APMPlayerControllerBase::CallOrRegister_OnExperienceLoaded(FOnExperienceLoaded::FDelegate&& Delegate)
{
	if (GetWorld() && GetWorld()->GetGameState())
	{
		UPMExperienceManagerComponent* ExperienceManager = GetWorld()->GetGameState()->FindComponentByClass<UPMExperienceManagerComponent>();
		if (ExperienceManager)
		{
			ExperienceManager->CallOrRegister_OnExperienceLoaded(MoveTemp(Delegate));
		}
	}
}

void APMPlayerControllerBase::Server_RespawnPlayer_Implementation()
{
	APawn* ControlledPawn = GetPawn();
	ChangeState(NAME_Inactive);
	if (ControlledPawn)
	{
		ControlledPawn->Destroy();
	}

	ServerRestartPlayer();
}

void APMPlayerControllerBase::CallOrRegister_OnPossessed(FOnPossessed::FDelegate&& Delegate)
{
	AMPlayerCharacterBase* MyCharacter = Cast<AMPlayerCharacterBase>(GetPawn());
	if (MyCharacter)
	{
		Delegate.Execute(MyCharacter);
	}
	else
	{
		Delegate_OnPossessed.Add(MoveTemp(Delegate));
	}
}

void APMPlayerControllerBase::CallOrRegister_OnReadyPlayerState(FOnReadyPlayState::FDelegate&& Delegate)
{
	if (PlayerState)
	{
		Delegate.Execute(PlayerState);
	}
	else
	{
		Delegate_OnReadyPlayerState.Add(MoveTemp(Delegate));
	}
}

APMPlayerState* APMPlayerControllerBase::GetPlayerState() const
{
	return Cast<APMPlayerState>(PlayerState);
}

UPMAbilitySystemComponent* APMPlayerControllerBase::GetAbilitySystemComponent() const
{
	const APMPlayerState* PMPlayerState = GetPlayerState();
	
	return PMPlayerState ? PMPlayerState->GetPMAbilitySystemComponent() : nullptr;
}

UMWidgetBase* APMPlayerControllerBase::AddOrRemoveWidget(const FGameplayTag& WidgetTag, bool bAddWidget, UObject* WidgetInstigator)
{
	UMViewportClient* VC = Cast<UMViewportClient>(GetGameInstance()->GetGameViewportClient());
	if (VC && bAddWidget)
	{
		return VC->AddWidgetToLayer(WidgetTag, { WidgetInstigator, this});
	}
	else if (VC && !bAddWidget)
	{
		VC->RemoveWidgetFromLayer(WidgetTag);
	}

	return nullptr;
}

UMWidgetBase* APMPlayerControllerBase::ToggleWidget(const FGameplayTag& WidgetTag, UObject* WidgetInstigator /*= nullptr*/)
{
	UMViewportClient* VC = Cast<UMViewportClient>(GetGameInstance()->GetGameViewportClient());
	if (VC)
	{
		return VC->ToggleWidgetOnLayer(WidgetTag, { WidgetInstigator, this });
	}

	return nullptr;
}

UMWidgetBase* APMPlayerControllerBase::GetWidgetInstance(const FGameplayTag& WidgetTag)
{
	UMViewportClient* VC = Cast<UMViewportClient>(GetGameInstance()->GetGameViewportClient());
	if (VC)
	{
		return VC->GetWidgetInstance(WidgetTag);
	}

	return nullptr;
}

void APMPlayerControllerBase::OnExperienceLoaded(const UPMExperienceDefinition* LoadedExperienceDefinition)
{
	const TSet<UActorComponent*>& SubComponents = GetComponents();
	for (UActorComponent* Component : SubComponents)
	{
		UMControllerComponentBase* SubComponent = Cast<UMControllerComponentBase>(Component);
		if (SubComponent)
		{
			SubComponent->OnExperienceLoaded(LoadedExperienceDefinition);
		}
	}
}
