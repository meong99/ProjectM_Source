// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "GameplayTagContainer.h"
#include "UIExtensionSystem.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "GameFeatureAction_AddWidgets.generated.h"

struct FComponentRequestHandle;

// 레이아웃 요청
USTRUCT()
struct FPMHUDLayoutRequest
{
	GENERATED_BODY()

	// 실제 UI의 클래스. Game UI라면 GameUI 위젯을 별도로 만들고, 그걸 레이아웃에 붙이는 형태
	UPROPERTY(EditAnywhere, Category = UI, meta = (AssetBundles = "Client"/*Remind 에셋 번들에서 읽기 위해 메타 키워드 추가*/))
	TSoftClassPtr<UCommonActivatableWidget> LayoutClass;

	UPROPERTY(EditAnywhere, Category = UI)
	FGameplayTag LayerID;
};

USTRUCT()
struct FPMHUDElementEntry
{
	GENERATED_BODY()

	// Layout 내부의 개별 위젯
	UPROPERTY(EditAnywhere, Category = UI, meta = (AssetBundles = "Client"/*에셋 번들에서 읽기 위해 메타 키워드 추가*/))
	TSoftClassPtr<UUserWidget> WidgetClass;

	UPROPERTY(EditAnywhere, Category = UI)
	FGameplayTag SlotID;
};

UCLASS()
class PROJECTM_API UGameFeatureAction_AddWidgets : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()
public:
	struct FPerContextData
	{
		TArray<TSharedPtr<FComponentRequestHandle>> ComponentRequests;
		TArray<TWeakObjectPtr<UCommonActivatableWidget>> LayoutsAdded;

		// HUD Element는 UIExtension으로 관리된다
		TArray<FUIExtensionHandle> ExtensionHandles;
	};

	void AddWidgets(AActor* Actor, FPerContextData& ActiveData);
	void RemoveWidgets(AActor* Actor, FPerContextData& ActiveData);


	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;

	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;

	void HandleActorExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);

	// GFA Add/Remove 상태 관리
	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;

	// Viewport에 어떤 Layout(Game, Menu, Popup...)을 장착할건지
	UPROPERTY(EditAnywhere, Category=UI)
	TArray<FPMHUDLayoutRequest> Layout;

	UPROPERTY(EditAnywhere, Category=UI)
	TArray<FPMHUDElementEntry> Widgets;
};
