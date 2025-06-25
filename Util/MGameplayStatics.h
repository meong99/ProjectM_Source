#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "GameplayTagContainer.h"
#include "Engine/HitResult.h"
#include "MGameplayStatics.generated.h"

class UMViewportClient;
class UPMExperienceManagerComponent;
class APMPlayerControllerBase;
class UMDataTableManager;
class UMDefinitionBase;
class UMWidgetBase;

UENUM(BlueprintType)
enum class EMCollisionChannel : uint8
{
    Monster		UMETA(DisplayName = "Monster"),
    Interaction	UMETA(DisplayName = "Interaction"),
	Player		UMETA(DisplayName = "Player"),
	NPC			UMETA(DisplayName = "NPC"),
	LavelTavel	UMETA(DisplayName = "LavelTavel"),
};

UCLASS()
class UMGameplayStatics : public UObject
{
	GENERATED_BODY()

public:
	static UPMExperienceManagerComponent*	GetExperienceManagerComponent(const UObject* WorldContext);
	static ECollisionChannel				GetCollisionChannel(EMCollisionChannel Channel);
	static UMDataTableManager*				GetDataTableManager();
	static void								SetTimerForNextTick(const UObject* WorldContext, TFunction<void()> Callback);
	static void								SetTimer(const UObject* WorldContext, TFunction<void()> Callback, float InRate, bool InbLoop, float InFirstDelay = -1.f);
	static bool								CheckNetGuid(const UObject* WorldContext, const UObject* Object);
	static FHitResult						GetFloorLocation(const UObject* WorldContext, const FVector& StartLocation, const TArray<AActor*>& ActorsToIgnore, const float Layrange = 1000.f);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static UMWidgetBase*	AddWidgetToLayer(const UObject* WorldContextObject, FGameplayTag WidgetTag, FMWidgetInfo InWidgetInfo);
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static UMWidgetBase*	RemoveWidgetFromLayer(const UObject* WorldContextObject, FGameplayTag WidgetTag);
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static UMWidgetBase*	GetWidgetInstance(const UObject* WorldContextObject, FGameplayTag WidgetTag);
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void				FadeIn(const UObject* WorldContextObject, const float Duration, const float FadeDelay = 0.f);
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void				FadeOut(const UObject* WorldContextObject, const float Duration, const float FadeDelay = 0.f);

	// 에디터일땐 에디터팝업, 아닐때는 로그
	static void	ShowErrorOrLog(const FString& Error);

	static bool bShowDebug_Console;
};
