#pragma once

#include "Components/MPawnComponentBase.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "PMPawnExtensionComponent.generated.h"

class UPMPawnData;
class UPMAbilitySystemComponent;

/*
* GameFeature시스템으로 추가하는 모든 컴포넌트의 초기화 과정을 담당한다.
* 이 컴포넌트가 최상위 계층에서 초기화를 담당하고, 초기화가 진행될때마다 하위 컴포넌트들에게 뿌리는 형태로 진행된다.
* 
* @Warning !! 이 컴포넌트는 무조건 C++ 생성자에서 생성해야한다!! 그리고 하위 컴포넌트는 꼭 BP에서 추가되어야만 한다!! 그렇지 않으면 초기화 순서 보장이 안된다!
*/
UCLASS()
class PROJECTM_API UPMPawnExtensionComponent : public UMPawnComponentBase, public IGameFrameworkInitStateInterface/*GameInstance에서 추가한 컴포넌트 초기화 과정을 도와주는 인터페이스*/
{
	GENERATED_BODY()

	/*
	* Overrided Function
	*/
public:
	UPMPawnExtensionComponent(const FObjectInitializer& ObjectInitializer);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual FName GetFeatureName() const final
	{
		return NAME_ActorFeatureName;
	}

private:
	virtual void OnRegister() final;
	virtual void BeginPlay() final;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) final;

	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) final;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const final;
	virtual void CheckDefaultInitialization() final;
/*
* Member Functions
*/
public:
	static UPMPawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor)
	{
		return IsValid(Actor) ? Actor->FindComponentByClass<UPMPawnExtensionComponent>() : nullptr;
	}

	void SetupPlayerInputComponent();

	void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);
	void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);

	void InitializeAbilitySystem(UPMAbilitySystemComponent* InAbilitySystemComponent, AActor* InOwnerActor);
	void UninitializeAbilitySystem();

	void HandleControllerChanged();

	template <class T = UPMPawnData>
	const T*					GetPawnData() const { return Cast<T>(PawnData); }
	UPMAbilitySystemComponent*	GetPMAbilitySystemComponent() const;

	void SetPawnData(const UPMPawnData* InPawnData);

private:
	UFUNCTION()
	void OnRep_PawnData();
/*
* Member Variables
*/
public:
	static const FName NAME_ActorFeatureName;

private:
	UPROPERTY(EditInstanceOnly, ReplicatedUsing = OnRep_PawnData, Category = "ProjectM | Pawn")
	TObjectPtr<const UPMPawnData> PawnData;

	UPROPERTY(VisibleAnywhere, Category = "ProjectM | Pawn")
	TObjectPtr<UPMAbilitySystemComponent> AbilitySystemComponent;

	FSimpleMulticastDelegate OnAbilitySystemInitialized;
	FSimpleMulticastDelegate OnAbilitySystemUninitialized;
};