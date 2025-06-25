// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MWidgetBase.h"
#include "MPlayerLevelBarWidget.generated.h"

class UTextBlock;
class UProgressBar;

UCLASS()
class PROJECTM_API UMPlayerLevelBarWidget : public UMWidgetBase
{
	GENERATED_BODY()

	/*
	* Overrided Functions
	*/
public:
	UMPlayerLevelBarWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeOnInitialized() override;

	/*
	* Member Functions
	*/
protected:
	UFUNCTION()
	void Callback_OnReadyPlayerState(APlayerState* PlayerState);
	void OnChange_Level(const int32 OldLevel, const int32 InCurrentLevel, const int64 InMaxExp);
	void OnChange_Exp(const int64 OldExp, const int64 InCurrentExp);
	/*
	* Member Variables
	*/
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentLevelText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentExpText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MaxExpText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> LevelBar;

	int32 CurrentLevel = 1;
	int64 CurrentExp = 0;
	int64 MaxExp = 1;
	FDelegateHandle Handle_ChangeLevel;
	FDelegateHandle Handle_ChangeExp;
};
