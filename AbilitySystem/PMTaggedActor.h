// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PMTaggedActor.generated.h"

UCLASS()
class PROJECTM_API APMTaggedActor : public AActor
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	APMTaggedActor();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

/*
* Member Functions
*/
public:

/*
* Member Variables
*/
private:
// 원래는 Actor에 맞는 Tag가 정의되도록 Interface를 상속받아 사용해야 하지만, 지금은 중요하지 않기 때문에 클래스 정의만 해두고, 추후 사용한다 
};