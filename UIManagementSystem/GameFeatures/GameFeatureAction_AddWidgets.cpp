#include "GameFeatureAction_AddWidgets.h"
#include "CommonUIExtensions.h"
#include "Components/GameFrameworkComponentManager.h"
#include "UI/PMHUD.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "GameFeaturesSubsystem.h"

void UGameFeatureAction_AddWidgets::AddWidgets(AActor* Actor, FPerContextData& ActiveData)
{
	APMHUD* HUD = CastChecked<APMHUD>(Actor);

	if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(HUD->GetOwningPlayerController()->Player))
	{
		for (const FPMHUDLayoutRequest& Entry : Layout)
		{
			// LayoutClass는 SoftClass라서 Loading을 해줘야만 한다.
			// 만약 이미 Loading되어있다면 SoftClass를 Get하면 Object가 반환되는데 안되어있다면 null이다.
			// 우선 위젯은 서버에서는 Loading할 필요가 없기 때문에  Loading은 이 곳에서 하지 않는다.
			// ExperienceManagerComponent에서 Bundle에서 Client로딩하는 곳에 위젯을 추가해서 미리 로딩을 해놓을 것이다.
			if (TSubclassOf<UCommonActivatableWidget> ConcreteWidgetClass = Entry.LayoutClass.Get())
			{
				ActiveData.LayoutsAdded.Add(UCommonUIExtensions::PushContentToLayer_ForPlayer(LocalPlayer, Entry.LayerID, ConcreteWidgetClass));
			}
		}

		UUIExtensionSubsystem* ExtensionSubsystem = HUD->GetWorld()->GetSubsystem<UUIExtensionSubsystem>();
		for (const FPMHUDElementEntry& Entry : Widgets)
		{
			ActiveData.ExtensionHandles.Add(ExtensionSubsystem->RegisterExtensionAsWidgetForContext(Entry.SlotID, LocalPlayer, Entry.WidgetClass.Get(), -1));
		}
	}
}

void UGameFeatureAction_AddWidgets::RemoveWidgets(AActor* Actor, FPerContextData& ActiveData)
{
	APMHUD* HUD = CastChecked<APMHUD>(Actor);

	for (TWeakObjectPtr<UCommonActivatableWidget>& AddedLayout : ActiveData.LayoutsAdded)
	{
		if (AddedLayout.IsValid())
		{
			AddedLayout->DeactivateWidget();
		}
	}
	ActiveData.LayoutsAdded.Reset();

	for (FUIExtensionHandle& Handle : ActiveData.ExtensionHandles)
	{
		Handle.Unregister();
	}
	ActiveData.ExtensionHandles.Reset();
}

void UGameFeatureAction_AddWidgets::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);
}

void UGameFeatureAction_AddWidgets::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
{
	UWorld* World = WorldContext.World();
	UGameInstance* GameInstance = WorldContext.OwningGameInstance;
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	// GameFrameworkComponentManager를 가져올 GameInstance와 World는 GameWorld여야한다
	if ((GameInstance != nullptr) && (World != nullptr) && World->IsGameWorld())
	{
		if (UGameFrameworkComponentManager* ComponentManager = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance))
		{
			// 기본적으로 Widget을 추가할 대상을 APMHUD로 고정한다.
			// ModularGameplayActor처럼 GameFeature에 등록하기위해 존재해야하는 Actor처럼 APMHUD을 만들어서 자동으로 등록되도록 할 것이다.
			TSoftClassPtr<AActor> HUDActorClass = APMHUD::StaticClass();

			// GFA_WorldBase와 마찬가지로 HandleActorExtension을 콜백으로 받는는다.
			TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle = ComponentManager->AddExtensionHandler(
				HUDActorClass,
				UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(this, &ThisClass::HandleActorExtension, ChangeContext));

			ActiveData.ComponentRequests.Add(ExtensionRequestHandle);
		}
	}
}

void UGameFeatureAction_AddWidgets::HandleActorExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext)
{
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved) || (EventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved))
	{
		RemoveWidgets(Actor, ActiveData);
	}
	else if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) || (EventName == UGameFrameworkComponentManager::NAME_GameActorReady))
	{
		AddWidgets(Actor, ActiveData);
	}
}
