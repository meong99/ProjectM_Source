#pragma once

#include "GameFramework/GameStateBase.h"
#include "GameplayTagContainer.h"
#include "PMGameStateBase.generated.h"

class UPMExperienceManagerComponent;
class USaveGame;
class UMTradeManager;

UCLASS()
class PROJECTM_API APMGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	APMGameStateBase();
	virtual void BeginPlay() override;

/*
* Member Functions
*/
public:
	UPMExperienceManagerComponent* GetExperienceManagerComponent() const { return ExperienceManagerComponent; }
	
	UFUNCTION(Server, Reliable)
	void Server_SaveGame();
	UFUNCTION(Server, Reliable)
	void Server_LoadGame();

public:
	UPROPERTY()
	TMap<FGameplayTag, AActor*> TagMappedActor;
/*
* Member Variables
*/
private:
	UPROPERTY()
	TObjectPtr<UPMExperienceManagerComponent> ExperienceManagerComponent;
	
	UPROPERTY()
	TObjectPtr<UMTradeManager> TradeManager;

	UPROPERTY()
	TArray<TObjectPtr<USaveGame>> PlayerSaveDataArray;
};