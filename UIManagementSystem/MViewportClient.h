// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "WidgetRegister/MWidgetRegister.h"
#include "Engine/GameViewportClient.h"
#include "MWidgetBase.h"
#include "Util/Stack.h"

#include "MViewportClient.generated.h"

class UMWidgetInstanceList;
class UMWidgetLayout;

/**
 * 
 */
UCLASS()
class PROJECTM_API UMViewportClient : public UGameViewportClient
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	virtual void Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice = true) override;
	virtual void PostRender(UCanvas* Canvas) override;
	virtual bool InputKey(const FInputKeyEventArgs& EventArgs) override;
	virtual bool WindowCloseRequested() override;

/*
* Member Functions
*/
public:
	static UMViewportClient* Get(const UObject* WorldContext);

	virtual void ClearFade();
	virtual void Fade(const float Duration, const bool InbToBlack, const float InFadeDelay = 0.f);
	void DrawScreenFade(UCanvas* Canvas);

	// 월드 기준 없이 전역적으로 사용되는 위젯의 Register를 Load, Caching해둔다.
	void LoadDefaultWidgetRegister();
	// 월드 단위로 사용되는 WidgetRegister를 등록, 해제한다. 이 부분은 GameFeatureAction으로 정의된다.
	void AddWidgetRegister(const FGameplayTag& RegisterTag, UMWidgetRegister* InWidgetRegister);
	void RemoveWidgetRegister(const FGameplayTag& RegisterTag);
	void ClearLayer();
	// Layout 적용
	void ApplyWidgetLayout();

	// Register내부에 있는 모든 위젯 생성
	UMWidgetInstanceList* CreateWidgetInRegister(const FGameplayTag& RegisterTag);

	// 해당하는 위젯을 바로 viewport에 설정
	void AddWidgetToViewport(const FGameplayTag& WidgetTag);
	void RemoveWidgetFromParent(const FGameplayTag& WidgetTag);
	void OnPressEsc();

	// Layer에 Widget을 설정한다.
	UMWidgetBase*	AddWidgetToLayer(const FGameplayTag& WidgetTag, const FMWidgetInfo& InWidgetInfo = {});
	UMWidgetBase*	RemoveWidgetFromLayer(const FGameplayTag& WidgetTag, const bool bIsDelete = false);
	UMWidgetBase*	ToggleWidgetOnLayer(const FGameplayTag& WidgetTag, const FMWidgetInfo& InWidgetInfo = {});

	// Widget의 Instance를 반환한다. 만약 Instancing되지 않았다면 Class를 찾아 Instancing후 반환한다.
	UMWidgetBase* GetWidgetInstance(const FGameplayTag& WidgetTag, const bool bIsDelete = false);

	UMWidgetRegister* GetWidgetRegister(const FGameplayTag& Tag);

private:
	UMWidgetInstanceList* CreateNewWidgetInstanceList(const FGameplayTag& RegisterTag);
/*
* Member Variables
*/

private:
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UMWidgetRegister>> WidgetRegisterMap;

	UPROPERTY()
	TMap<FGameplayTag/*RegisterTag*/, TObjectPtr<UMWidgetInstanceList>> WidgetInstanceListMap;

	UPROPERTY()
	UMWidgetLayout* WidgetLayout;

	uint32 bFading : 1;
	uint32 bToBlack : 1;
	float FadeAlpha;
	float FadeStartTime;
	float FadeDuration;
	float FadeDelay;
};
