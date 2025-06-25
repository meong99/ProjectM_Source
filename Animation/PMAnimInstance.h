// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayEffectTypes.h"

#include "PMAnimInstance.generated.h"

class UAbilitySystemComponent;
class AMCharacterBase;
class UCharacterMovementComponent;

// BP에 있는거 전부 Native화 해야함
UCLASS()
class PROJECTM_API UPMAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UPMAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	virtual void NativeInitializeAnimation() override;

/*
* Member Functions
*/
public:
	void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);

/*
* Member Variables
*/
private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = ture), Category = "ProjectM")
	float GroundDistance = -1.0f;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = ture), Category = "ProjectM")
	FVector Velocity;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = ture), Category = "ProjectM")
	bool bIsFalling = false;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = ture), Category = "ProjectM")
	bool bCanMove = true;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = ture), Category = "ProjectM")
	TObjectPtr<AMCharacterBase> CharacterBase;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = ture), Category = "ProjectM")
	UCharacterMovementComponent* MovementComponent;

	// GameplayTag와 AnimInstance의 속성값을 매핑한다.
	// ex) Tag와 bIsShotFire와 같은 변수를 매핑해서 Tag가 활성화되면 bIsShotFire가 ture가 되고, Tag가 비활성화되면 bIsShotFire가 false가 된다.
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = ture), Category = "ProjectM | GameplayTags")
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;
};