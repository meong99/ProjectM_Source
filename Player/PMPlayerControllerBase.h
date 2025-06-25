#pragma once

#include "CommonPlayerController.h"
#include "GameModes/PMExperienceManagerComponent.h"
#include "Character/MPlayerCharacterBase.h"
#include "PMPlayerControllerBase.generated.h"

class APMPlayerState;
class UPMAbilitySystemComponent;
class UPMInventoryManagerComponent;
class UMPlayerTradeComponent;
class UMPlayerQuestComponent;
class UPMExperienceDefinition;
class UMWidgetBase;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPossessed, AMPlayerCharacterBase* Character);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnReadyPlayState, APlayerState* PlayerState);

UCLASS()
class PROJECTM_API APMPlayerControllerBase : public ACommonPlayerController
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	APMPlayerControllerBase();

	virtual void OnPossess(APawn* aPawn) override;
	virtual void BeginPlay() override;
	virtual void BeginPlayingState() override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void ServerRestartPlayer_Implementation() override;
	virtual void ClientRestart_Implementation(APawn* NewPawn) override;
	virtual void BeginDestroy() override;

/*
* Member Functions
*/
public:
	void CallOrRegister_OnExperienceLoaded(FOnExperienceLoaded::FDelegate&& Delegate);
	void CallOrRegister_OnPossessed(FOnPossessed::FDelegate&& Delegate);
	void CallOrRegister_OnReadyPlayerState(FOnReadyPlayState::FDelegate&& Delegate);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_RespawnPlayer();

	APMPlayerState* GetPlayerState() const;

	UFUNCTION(BlueprintCallable)
	UPMAbilitySystemComponent* GetAbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	UMWidgetBase* AddOrRemoveWidget(const FGameplayTag& WidgetTag, bool bAddWidget, UObject* WidgetInstigator = nullptr);
	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	UMWidgetBase* ToggleWidget(const FGameplayTag& WidgetTag, UObject* WidgetInstigator = nullptr);
	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	UMWidgetBase* GetWidgetInstance(const FGameplayTag& WidgetTag);

protected:
	void OnExperienceLoaded(const UPMExperienceDefinition* LoadedExperienceDefinition);

/*
* Member Variables
*/
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UPMInventoryManagerComponent> InventoryManagerComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UMPlayerTradeComponent> PlayerTradeComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UMPlayerQuestComponent> PlayerQuestComponent;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<APawn> OldPawn;

	FOnPossessed		Delegate_OnPossessed;
	FOnReadyPlayState	Delegate_OnReadyPlayerState;
};