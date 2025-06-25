#include "MLevelManager.h"
#include "GameModes/PMUserFacingExperienceDefinition.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "CommonSessionSubSystem.h"
#include "Engine/LocalPlayer.h"
#include "UI/MViewportClient.h"
#include "Kismet/GameplayStatics.h"

UMLevelManager::UMLevelManager()
{
}

void UMLevelManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UMLevelManager::OnLevelLoaded);
}

void UMLevelManager::TravelLevel(UPMUserFacingExperienceDefinition* UFED, const FString& Ip, bool bIsSinglePlay)
{
	CurrentUFED = UFED;
	if (CurrentUFED)
	{
		UCommonSessionSubSystem* Session = GetSessionSubsystem();
		if (Session)
		{
			UMViewportClient* ViewportClient = Cast<UMViewportClient>(GetWorld()->GetGameInstance()->GetGameViewportClient());
			if (ViewportClient)
			{
				ViewportClient->ClearLayer();
			}
 			Session->HostSession(GetPlayerController(), CurrentUFED->CreateHostingRequst(Ip, bIsSinglePlay));
		}
	}
	else
	{
		MCHAE_ERROR("UserFacingExperienceDefinition is not valid!!!!!!!");
		ensure(false);
	}
}

void UMLevelManager::OnLevelLoaded(UWorld* NewWorld)
{
	/*UGameInstance* GameInstance = NewWorld->GetGameInstance();
	if (GameInstance)
	{
		UMViewportClient* ViewportClient = Cast<UMViewportClient>(GameInstance->GetGameViewportClient());
		if (ViewportClient)
		{
			ViewportClient->ApplyWidgetLayout();
		}
	}*/
}

UCommonSessionSubSystem* UMLevelManager::GetSessionSubsystem() const
{
	UWorld* World = GetWorld();
	UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	UCommonSessionSubSystem* Session = GameInstance ? GameInstance->GetSubsystem<UCommonSessionSubSystem>() : nullptr;

	return Session;
}

APlayerController* UMLevelManager::GetPlayerController() const
{
	ULocalPlayer* Local = GetLocalPlayer();
	if (Local)
	{
		return Local->GetPlayerController(GetWorld());
	}

	return nullptr;
}
