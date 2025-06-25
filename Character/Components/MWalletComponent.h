// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/MControllerComponentBase.h"
#include "MWalletComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChangeGold, int64, AdjustNum, int64, NewGold);

UCLASS()
class PROJECTM_API UMWalletComponent : public UMControllerComponentBase
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UMWalletComponent(const FObjectInitializer& ObjectInitializer);
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	/*
* Member Functions
*/
public:
	UFUNCTION(BlueprintCallable)
	void SubtractGold(int64 AdjustGold);

	UFUNCTION(BlueprintCallable)
	void AddGold(int64 AdjustGold);

	UFUNCTION(BlueprintCallable)
	int64 GetGold() const { return Gold; }

protected:
	UFUNCTION()
	void OnRep_OnChangeGold(const int64 OldGold);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerDebug_AddGold(int64 AdjustGold);
	UFUNCTION(Exec)
	void Debug_AddGold(int64 AdjustGold);
	UFUNCTION(Server, Reliable)
	void ServerDebug_SubtractGold(int64 AdjustGold);
	UFUNCTION(Exec)
	void Debug_SubtractGold(int64 AdjustGold);
/*
* Member Variables
*/
public:
	UPROPERTY(BlueprintAssignable)
	FOnChangeGold Delegate_OnChangeGold;

protected:
	UPROPERTY(ReplicatedUsing = "OnRep_OnChangeGold", meta = (ClampMin = 0))
	int64 Gold = 0;
};
