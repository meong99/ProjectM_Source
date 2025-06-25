#pragma once

#include "ModularCharacter.h"
#include "AbilitySystemInterface.h"
#include "Types/MCharacterTypes.h"
#include "Animation/AnimInstance.h"
#include "GenericTeamAgentInterface.h"
#include "MCharacterBase.generated.h"

class UMNameWidgetComponent;
class UPMAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnChangeCharacterStateFlags, const int64& OldFlags, const int64& NewFlags);

UCLASS()
class PROJECTM_API AMCharacterBase : public AModularCharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	AMCharacterBase(const FObjectInitializer& ObjectInitializer);
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual bool IsMoveInputIgnored() const;

/*
* Member Functions
*/
public:
	virtual UAbilitySystemComponent*	GetAbilitySystemComponent() const override { return nullptr; }
	virtual UPMAbilitySystemComponent*	GetMAbilitySystemComponent() const { return nullptr; }
	virtual void InitCharacterName();
	virtual void OnDead();

	const FName&	GetCharacterName() const;
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void	Server_SetCharacterLifeState(const EMCharacterLiftState InState);
	UFUNCTION(BlueprintCallable)
	const EMCharacterLiftState& GetCharacterLifeState() const { return CharacterLifeState; }
	UFUNCTION(BlueprintCallable)
	bool	IsOnCharacterStateFlags(const int64& InState) const	{ return CharacterStateFlag & InState; }
	UFUNCTION(BlueprintCallable)
	int64	GetCharacterStateFlag() const					{ return CharacterStateFlag; }
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void	Server_AddCharacterStateFlag(const int64& InState);
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void	Server_RemoveCharacterStateFlag(const int64& InState);

	
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override { TeamId = InTeamID; }
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

protected:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayMontage(UAnimMontage* MontageToPlay, float InPlayRate = 1.f, EMontagePlayReturnType ReturnValueType = EMontagePlayReturnType::MontageLength, float InTimeToStartMontageAt = 0.f, bool bStopAllMontages = true);

	UFUNCTION()
	void OnRep_OnChangeStateFlags(const int64& OldFlag);
/*
* Member Variables
*/
public:
	FOnChangeCharacterStateFlags Delegate_OnChangeCharacterStateFlags;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Name")
	UMNameWidgetComponent* NameComponent;

	UPROPERTY(BlueprintReadWrite)
	FName CharacterName = TEXT("None");

	UPROPERTY(Replicated, BlueprintReadWrite)
	EMCharacterLiftState CharacterLifeState = EMCharacterLiftState::WaitToSpawn;

	UPROPERTY(ReplicatedUsing = OnRep_OnChangeStateFlags, BlueprintReadWrite)
	int64 CharacterStateFlag = EMCharacterStateFlag::None_M;

	UPROPERTY()
	FGenericTeamId TeamId;
};