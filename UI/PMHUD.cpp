#include "PMHUD.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "GameModes/PMExperienceManagerComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(PMHUD)

APMHUD::APMHUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void APMHUD::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void APMHUD::BeginPlay()
{
	AGameStateBase* State = GetWorld()->GetGameState();
	UPMExperienceManagerComponent* ExperienceManagerComp = State ? State->FindComponentByClass<UPMExperienceManagerComponent>() : nullptr;
	if (ExperienceManagerComp)
	{
		ExperienceManagerComp->CallOrRegister_OnExperienceLoaded(FOnExperienceLoaded::FDelegate::CreateWeakLambda(this, [this](const UPMExperienceDefinition* LoadedExperienceDefinition)->void
			{
				UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);
			}));
	}

	Super::BeginPlay();
}

void APMHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);
	Super::EndPlay(EndPlayReason);
}
