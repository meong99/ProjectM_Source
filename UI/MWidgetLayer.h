// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Overlay.h"
#include "Util/Stack.h"
#include "GameplayTagContainer.h"
#include "MWidgetBase.h"
#include "MWidgetLayer.generated.h"

UCLASS()
class PROJECTM_API UMWidgetLayer : public UOverlay
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UMWidgetLayer(const FObjectInitializer& ObjectInitializer);

/*
* Member Functions
*/
public:
	void AddWidgetToLayer(UMWidgetBase* Widget);
	void RemoveWidgetFromLayer(UMWidgetBase* Widget);
	void ActivateLayer();
	void ClearAllWidget();
	bool RemoveTopWidgetInGameLayer();

protected:
	UOverlay* GetAppropriateOverlay(const int32 Id);
	void SetInputMode(UMWidgetBase* Widget = nullptr) const;
	void PushInputMode(EMWidgetInputMode Input, const FGameplayTag& Tag);
	void PopInputMode(EMWidgetInputMode Input);

	EMWidgetInputMode GetTopInputMode(EMWidgetInputMode Input = EMWidgetInputMode::GameAndUIWithShowMouse) const;

/*
* Member Variables
*/
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ProjectM")
	bool bIsActiveByDefault = false;

	EMWidgetInputMode CurrentTopInputMode = EMWidgetInputMode::GameAndUIWithShowMouse;
	EMWidgetInputMode OldTopInputMode = EMWidgetInputMode::None;

	TMap<EMWidgetInputMode, TStack<FGameplayTag>> WidgetMapByInputMode;
};
