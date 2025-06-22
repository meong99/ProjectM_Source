// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "GameFeatureAction_AddRegisterWidget.generated.h"

struct FComponentRequestHandle;
class UMWidgetRegister;

USTRUCT()
struct FMWidgetRegisterHandle
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = UI, meta = (AssetBundles = "Client"))
	TObjectPtr<UMWidgetRegister> WidgetRegister;
};

UCLASS()
class PROJECTM_API UGameFeatureAction_AddRegisterWidget : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;

/*
* Member Functions
*/
public:
	struct FPerContextData
	{
		TSharedPtr<FComponentRequestHandle> ComponentRequests;

		UPROPERTY()
		UGameInstance* GameInstance;
	};
	void HandleWidgetRegistry(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);

private:
	void AddWidgetRegister(FGameFeatureStateChangeContext ChangeContext);
	void RemoveWidgetRegister(FGameFeatureStateChangeContext ChangeContext);

/*
* Member Variables
*/
protected:
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TArray<FMWidgetRegisterHandle> WidgetRegisterHandles;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	bool bAddToLayer = false;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	int32 LayerId = 0;

	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;
};
