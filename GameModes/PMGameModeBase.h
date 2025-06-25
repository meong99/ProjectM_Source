// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/GameModeBase.h"

#include "PMGameModeBase.generated.h"

class UPMPawnData;
class UPMExperienceDefinition;

UCLASS(Config = Game)
class APMGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	APMGameModeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void	InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void	InitGameState() override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual void	HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) final;
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) final;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	/*
* Member Functions
*/
public:
	bool IsExperienceLoaded() const;
	const UPMPawnData* GetPawnDataForController(const AController* InController) const;

protected:
	// 정해진 기준에 따라 ExperienceDefinotion을 찾아서 ExperienceManager에 세팅해준다.
	void HandleMatchAssignmentIfNotExpectingOne();

	// Experience Manager에 찾아낸 ExperienceDefinition을 설정해준다.
	void OnMatchAssignmentGiven(const FPrimaryAssetId& ExperienceId);

	void OnExperienceLoaded(const UPMExperienceDefinition* CurrentExperience);

	bool TryDedicatedServerLogin();
/*
* Member Variables
*/

protected:
};
