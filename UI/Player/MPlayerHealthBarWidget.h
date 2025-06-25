// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MWidgetBase.h"
#include "AttributeSet.h"
#include "MPlayerHealthBarWidget.generated.h"

class UImage;
class APawn;
class UPMHealthComponent;
class AActor;
class UProgressBar;
class UTextBlock;

UCLASS()
class PROJECTM_API UMPlayerHealthBarWidget : public UMWidgetBase
{
	GENERATED_BODY()

	/*
	* Overrided Functions
	*/
public:
	UMPlayerHealthBarWidget(const FObjectInitializer& ObjectInitializer);

	virtual void	NativeConstruct() override;

/*
* Member Functions
*/
protected:
	UFUNCTION()
	void Callback_OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	UFUNCTION()
	void Callback_OnHealthChanged(const FGameplayAttribute& Attribute, UPMHealthComponent* HealthComp, const float OldValue, const float NewValue, AActor* Instigator);
	FText MakeFormatText(const float Health, const float MaxHealth) const;
/*
* Member Variables
*/
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HealthText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HealthRecoveryText;
};
