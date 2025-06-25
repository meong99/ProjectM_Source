#include "MGameplayStatics.h"
#include "UI/MViewportClient.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "GameFramework/GameStateBase.h"
#include "GameModes/PMExperienceManagerComponent.h"
#include "Misc/MessageDialog.h"
#include "Engine/DataTable.h"
#include "System/MDataTableManager.h"
#include "Table/MTable_TableBase.h"
#include "TimerManager.h"
#include "Engine/NetDriver.h"
#include "Engine/PackageMapClient.h"
#include "Kismet/KismetSystemLibrary.h"

bool UMGameplayStatics::bShowDebug_Console = false;
static FAutoConsoleVariableRef CVarMyBoolVar(
	TEXT("ProjectM.Debug.ShowDebug"), // 콘솔 변수 이름
	UMGameplayStatics::bShowDebug_Console, // 변수와 연동
	TEXT("디버그 옵션을 켤지 말지"), // 설명
	ECVF_Default // 플래그 (옵션)
);

UWorld* GetWorldFromContext(const UObject* WorldContext)
{
	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull) : nullptr;

	return World;
}

UPMExperienceManagerComponent* UMGameplayStatics::GetExperienceManagerComponent(const UObject* WorldContext)
{
	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull) : nullptr;
	if (World)
	{
		AGameStateBase* GameState = World->GetGameState();
		if (GameState)
		{
			return GameState->FindComponentByClass<UPMExperienceManagerComponent>();
		}
	}

	return nullptr;
}

ECollisionChannel UMGameplayStatics::GetCollisionChannel(EMCollisionChannel Channel)
{
	switch (Channel)
	{
		case EMCollisionChannel::Monster	:	return ECollisionChannel::ECC_GameTraceChannel1;
		case EMCollisionChannel::Interaction:	return ECollisionChannel::ECC_GameTraceChannel2;
		case EMCollisionChannel::Player		:	return ECollisionChannel::ECC_GameTraceChannel3;
		case EMCollisionChannel::NPC		:	return ECollisionChannel::ECC_GameTraceChannel4;
		case EMCollisionChannel::LavelTavel	:	return ECollisionChannel::ECC_GameTraceChannel5;
		default								:	return ECC_Visibility;
	}
}

UMDataTableManager* UMGameplayStatics::GetDataTableManager()
{
	if (!GEngine)
	{
		ensure(false);
		MCHAE_WARNING("Can't Access GEngine!!");
		return nullptr;
	}

	return GEngine->GetEngineSubsystem<UMDataTableManager>();
}

void UMGameplayStatics::SetTimerForNextTick(const UObject* WorldContext, TFunction<void()> Callback)
{
	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull) : nullptr;
	check(World);
	World->GetTimerManager().SetTimerForNextTick(MoveTemp(Callback));
}

void UMGameplayStatics::SetTimer(const UObject* WorldContext, TFunction<void()> Callback, float InRate, bool InbLoop, float InFirstDelay)
{
	FTimerHandle Handle;
	UWorld* World = GetWorldFromContext(WorldContext);
	if (!World)
	{
		ensure(false);
		return;
	}

	World->GetTimerManager().SetTimer(Handle, MoveTemp(Callback), InRate, InbLoop, InFirstDelay);
}

bool UMGameplayStatics::CheckNetGuid(const UObject* WorldContext, const UObject* Object)
{
	UWorld* World = GetWorldFromContext(WorldContext);
	if (!World) return false;

	UNetDriver* NetDriver = World->GetNetDriver();
	if (!NetDriver) return false;

	TSharedPtr<FNetGUIDCache> GuidCache = NetDriver->GuidCache;
	if (!GuidCache) return false;

	FNetworkGUID Guid = GuidCache->GetNetGUID(Object);
	return Guid.IsValid();
}

FHitResult UMGameplayStatics::GetFloorLocation(const UObject* WorldContext, const FVector& StartLocation, const TArray<AActor*>& ActorsToIgnore, const float Layrange /*= 1000.f*/)
{
	EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;
	if (UMGameplayStatics::bShowDebug_Console)
	{
		DebugType = EDrawDebugTrace::ForDuration;
	}

	FHitResult OutHit;
	UKismetSystemLibrary::LineTraceSingle(WorldContext, StartLocation,
										  StartLocation + FVector::DownVector * Layrange, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_WorldStatic),
										  false, ActorsToIgnore, DebugType, OutHit, true);
	return OutHit;
}

UMWidgetBase* UMGameplayStatics::AddWidgetToLayer(const UObject* WorldContextObject, FGameplayTag WidgetTag, FMWidgetInfo InWidgetInfo)
{
	UMViewportClient* ViewportClient = UMViewportClient::Get(WorldContextObject);
	if (ViewportClient)
	{
		return ViewportClient->AddWidgetToLayer(WidgetTag, InWidgetInfo);
	}

	return nullptr;
}

UMWidgetBase* UMGameplayStatics::RemoveWidgetFromLayer(const UObject* WorldContextObject, FGameplayTag WidgetTag)
{
	UMViewportClient* ViewportClient = UMViewportClient::Get(WorldContextObject);
	if (ViewportClient)
	{
		return ViewportClient->RemoveWidgetFromLayer(WidgetTag);
	}

	return nullptr;
}

UMWidgetBase* UMGameplayStatics::GetWidgetInstance(const UObject* WorldContextObject, FGameplayTag WidgetTag)
{
	UMViewportClient* ViewportClient = UMViewportClient::Get(WorldContextObject);
	if (ViewportClient)
	{
		return ViewportClient->GetWidgetInstance(WidgetTag);
	}

	return nullptr;
}

void UMGameplayStatics::FadeIn(const UObject* WorldContextObject, const float Duration, const float FadeDelay)
{
	UMViewportClient* ViewClient = UMViewportClient::Get(WorldContextObject);
	if (ViewClient)
	{
		ViewClient->Fade(Duration, false, FadeDelay);
	}
}

void UMGameplayStatics::FadeOut(const UObject* WorldContextObject, const float Duration, const float FadeDelay)
{
	UMViewportClient* ViewClient = UMViewportClient::Get(WorldContextObject);
	if (ViewClient)
	{
		ViewClient->Fade(Duration, true, FadeDelay);
	}
}

void UMGameplayStatics::ShowErrorOrLog(const FString& Error)
{
#if WITH_EDITOR
	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(Error));
#else
	MCHAE_ERROR("%s", *Error);
#endif
}

