// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "MWidgetInstanceList.generated.h"

class UMWidgetBase;
class UMWidgetRegister;

USTRUCT()
struct FMWidgetInstanceMapWrapper
{
	GENERATED_BODY();

	UPROPERTY()
	TMap<FGameplayTag/*WidgetTag*/, TObjectPtr<UMWidgetBase>> WidgetInstanceMap;
};

/*
* Instancing된 Widget을 관리하는 클래스
*/
UCLASS()
class PROJECTM_API UMWidgetInstanceList : public UObject
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UMWidgetInstanceList(const FObjectInitializer& ObjectInitializer);

/*
* Member Functions
*/
public:
	static UMWidgetInstanceList* CreateNewWidgetInstanceList(UObject* WorldContextObject, const FGameplayTag& RegisterTag);

	void CreateNewWidgets(UMWidgetRegister* WidgetRegister);
	UMWidgetBase* CreateNewWidget(const FGameplayTag& InWidgetTag, TSubclassOf<UMWidgetBase> WidgetClass);

	UMWidgetBase* GetWidgetInstance(const FGameplayTag& WidgetTag) const;

	const FGameplayTag&					GetRegisterTag() const { return RegisterTag; }
	const FMWidgetInstanceMapWrapper*	GetWidgetInstanceMapWrapper() const { return &WidgetInstanceMapWrapper; }

/*
* Member Variables
*/
protected:
	UPROPERTY()
	FMWidgetInstanceMapWrapper WidgetInstanceMapWrapper;

	UPROPERTY()
	FGameplayTag RegisterTag;
};
