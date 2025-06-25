// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "MWidgetBase.generated.h"

class AActor;
class APMPlayerControllerBase;

UENUM(BlueprintType)
enum class EMWidgetLayout : uint8
{
	// 게임 플레이 중 나오는 Layout. (HUD, Inventory ...)
	GameLayer UMETA(DisplayName = "GameLayer"),

	// 다른 위젯 없이 단독으로 보여지는 위젯 레이아웃. 단 하나씩만 설정 가능하다.
	IndependentLayer UMETA(DisplayName = "IndependentLayer"),

	None UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
enum class EMWidgetLayerId : uint8
{
	// 가장 뒷단의 HUD위젯
	Hud UMETA(DisplayName = "Hud"),

	// 인터렉션 관련 위젯
	Interaction UMETA(DisplayName = "Interaction"),

	// 인게임에서 인터렉티브한 모든 UI. Esc로 제거 가능. Ex) Inventory, Quest...
	Game UMETA(DisplayName = "Game"),

	// 로딩에 사용. 가장 앞에 그려진다.
	Loading UMETA(DisplayName = "Loading"),

	None UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
enum class EMWidgetInputMode : uint8
{
	GameOnly,
	GameAndUIWithShowMouse,
	UIOnly,
	None,
};

USTRUCT(BlueprintType)
struct FMWidgetInfo
{
	GENERATED_BODY()
	FMWidgetInfo(UObject* InWidgetInstigator = nullptr, AActor* InWidgetOwnerActor = nullptr)
	: WidgetInstigator(InWidgetInstigator)
	, WidgetOwnerActor(InWidgetOwnerActor)
	{}

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetInstigator;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor>	WidgetOwnerActor;
};

UCLASS()
class PROJECTM_API UMWidgetBase : public UUserWidget
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UMWidgetBase(const FObjectInitializer& ObjectInitializer);

	virtual void	NativeOnInitialized() override;
	virtual void	NativeConstruct();
	virtual void	NativeDestruct();
	virtual FReply	NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply	NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

/*
* Member Functions
*/
public:
	UFUNCTION(BlueprintCallable)
	void AddWidgetToLayer(const FMWidgetInfo& InWidgetInfo);
	UFUNCTION(BlueprintCallable)
	void RemoveWidgetFromLayer();

	void CallPreAddToLayer();

	const FGameplayTag& GetWidgetTag() const { return WidgetTag; }
	bool				IsActivate() const { return bIsActivate; }
	EMWidgetInputMode	GetInputMode() const { return InputMode; }
	const FMWidgetInfo&	GetWidgetInfo() const { return WidgetInfo; }

	void SetWidgetTag(const FGameplayTag& InWidgetTag) { WidgetTag = InWidgetTag; }
	void SetActivate(const bool bNewActivate) { bIsActivate = bNewActivate; }
	virtual void SetWidgetInfo(const FMWidgetInfo& InWidgetInfo) { WidgetInfo = InWidgetInfo; }
	void SetInitialized(bool bInIsInitialized) { bIsInitialized = bInIsInitialized; }
	
	UFUNCTION(BlueprintCallable)
	bool			IsInLayer() const { return bIsActivate; }
	bool			IsInitialized() const { return bIsInitialized; }
	EMWidgetLayerId GetWidgetLayerId() const { return WidgetLayerId; }
	EMWidgetLayout	GetWidgetLayout() const { return WidgetLayout; }

	APMPlayerControllerBase* GetPlayerController() const;

protected:
	// 생성되고 레이어에 등록되기 직전 한 번만 호출됨. WidgetInstigator등 커스텀 변수 사용한다면 NativeOnInitialized함수 대신 사용
	virtual void PreAddToLayer();
/*
* Member Variables
*/
protected:
	UPROPERTY(EditDefaultsOnly, Category="ProjectM")
	EMWidgetInputMode	InputMode = EMWidgetInputMode::GameAndUIWithShowMouse;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectM")
	EMWidgetLayerId		WidgetLayerId = EMWidgetLayerId::Game;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectM")
	EMWidgetLayout		WidgetLayout = EMWidgetLayout::GameLayer;

	UPROPERTY(VisibleInstanceOnly, Category = "ProjectM")
	FGameplayTag		WidgetTag;

	UPROPERTY(BlueprintReadWrite, Category = "ProjectM")
	FMWidgetInfo		WidgetInfo;

	// Layer에 등록된 위젯의 활성화 여부
	UPROPERTY(BlueprintReadOnly, Category = "ProjectM")
	bool bIsActivate = false;

	UPROPERTY(BlueprintReadOnly, Category = "ProjectM")
	bool bIsInitialized = false;
};
