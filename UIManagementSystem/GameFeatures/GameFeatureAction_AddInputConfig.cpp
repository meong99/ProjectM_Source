#include "GameFeatureAction_AddInputConfig.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Pawn.h"
#include "Character/Components/PMCharacterInitComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Engine/GameInstance.h"

UGameFeatureAction_AddInputConfig::UGameFeatureAction_AddInputConfig()
{
}

void UGameFeatureAction_AddInputConfig::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	// PIE상황에서 문제가 발생할 수도 있으니 한 번 더 체크하자는 의미
	FPerContextData& ActiveData = ContextData.FindOrAdd(Context);
	if (!ensure(ActiveData.ExtensionRequestHandles.IsEmpty()) ||
		!ensure(ActiveData.PawnsAddedTo.IsEmpty()))
	{
		Reset(ActiveData);
	}

	Super::OnGameFeatureActivating(Context);
}

void UGameFeatureAction_AddInputConfig::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	FPerContextData* ActiveData = ContextData.Find(Context);
	if (ensure(ActiveData))
	{
		Reset(*ActiveData);
	}
}

void UGameFeatureAction_AddInputConfig::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
{
	UWorld* World = WorldContext.World();
	UGameInstance* GameInstance = WorldContext.OwningGameInstance;
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	if (GameInstance && World && World->IsGameWorld())
	{
		if (UGameFrameworkComponentManager* ComponentManager = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance))
		{
			// 아래 Extionsion은 이벤트 방식으로서 APawn::StaticClass()에 대해 Extention 이벤트 Call이 오면 뒤에 정의한 Delegate를 호출하도록 한다.
			// CharacterInitComp에서 초기화가 완료된 시점인 InitializePlayerInput에서 이벤트를 호출해준다.
			UGameFrameworkComponentManager::FExtensionHandlerDelegate AddConfigDelegate =
				UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(this, &ThisClass::HandlePawnExtension, ChangeContext);

			TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle = ComponentManager->AddExtensionHandler(APawn::StaticClass(), AddConfigDelegate);
			ActiveData.ExtensionRequestHandles.Add(ExtensionRequestHandle);
		}
	}
}

void UGameFeatureAction_AddInputConfig::HandlePawnExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext)
{
	APawn* AsPawn = CastChecked<APawn>(Actor);
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	if (EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded || EventName == UPMCharacterInitComponent::NAME_BindInputsNow)
	{
		AddInputConfig(AsPawn, ActiveData);
	}
	else if (EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved)
	{
		RemoveInputConfig(AsPawn, ActiveData);
	}
}

void UGameFeatureAction_AddInputConfig::AddInputConfig(APawn* Pawn, FPerContextData& ActiveData)
{
	APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController());
	if (ULocalPlayer* LocalPlayer = PlayerController ? PlayerController->GetLocalPlayer() : nullptr)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			FModifyContextOptions Options = {};
			Options.bIgnoreAllPressedKeysUntilRelease = false;

			for (const FPMMappableConfigPair& Pair : InputConfigs)
			{
				if (Pair.bShouldActivateAutomatically)
				{
					Subsystem->AddPlayerMappableConfig(Pair.Config.LoadSynchronous(), Options);
				}
			}

			// Pawn캐싱. 
			ActiveData.PawnsAddedTo.AddUnique(Pawn);
		}
	}
}

void UGameFeatureAction_AddInputConfig::RemoveInputConfig(APawn* Pawn, FPerContextData& ActiveData)
{
	APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController());
	if (ULocalPlayer* LocalPlayer = PlayerController ? PlayerController->GetLocalPlayer() : nullptr)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			FModifyContextOptions Options = {};
			Options.bIgnoreAllPressedKeysUntilRelease = false;

			for (const FPMMappableConfigPair& Pair : InputConfigs)
			{
				if (Pair.bShouldActivateAutomatically)
				{
					Subsystem->RemovePlayerMappableConfig(Pair.Config.LoadSynchronous(), Options);
				}
			}

			// 캐싱된 Pawn 제거
			ActiveData.PawnsAddedTo.Remove(Pawn);
		}
	}
}

void UGameFeatureAction_AddInputConfig::Reset(FPerContextData& ActiveData)
{
	ActiveData.ExtensionRequestHandles.Empty();

	while (!ActiveData.PawnsAddedTo.IsEmpty())
	{
		TWeakObjectPtr<APawn> Pawn = ActiveData.PawnsAddedTo.Top();
		if (Pawn.IsValid() && Pawn->GetController())
		{
			RemoveInputConfig(Pawn.Get(), ActiveData);
		}
		else
		{
			ActiveData.PawnsAddedTo.Pop();
		}
	}
}
