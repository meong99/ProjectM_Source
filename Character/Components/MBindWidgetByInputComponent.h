// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MWidgetInstanceList.h"
#include "Components/MPawnComponentBase.h"

#include "MBindWidgetByInputComponent.generated.h"

class UMWidgetInstanceList;
class UPMInputComponent;

/*
* 입력에 따라 위젯이 켜지고, 꺼지는 동작을 수행할 수 있도록 하는 컴포넌트
* 위젯을 다루는 클래스라서 클라이언트 전용 클래스다
*/
UCLASS()
class PROJECTM_API UMBindWidgetByInputComponent : public UMPawnComponentBase
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UMBindWidgetByInputComponent(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;

	/*
* Member Functions
*/
public:

private:
	void BindWidgetByInput(UInputComponent* InInputComponent);

	void ToggleWidget(const FGameplayTag& Tag) const;

	UMWidgetInstanceList*	GetWidgetInstanceList() const;
/*
* Member Variables
*/
private:
	const FMWidgetInstanceMapWrapper* WidgetInstanceMapWrapper;

	UPROPERTY()
	UPMInputComponent* InputComponent;
};
