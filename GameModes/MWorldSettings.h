#pragma once

#include "GameFramework/WorldSettings.h"
#include "GameplayTagContainer.h"
#include "MWorldSettings.generated.h"

class UPMExperienceDefinition;

UCLASS()
class PROJECTM_API AMWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	AMWorldSettings(const FObjectInitializer& ObjectInitializer);

/*
* Member Functions
*/
public:
	FPrimaryAssetId GetDefaultGameplayExperience() const;

/*
* Member Variables
*/
public:
	UPROPERTY(EditDefaultsOnly, Category = "ProjectM")
	TSoftClassPtr<UPMExperienceDefinition> DefaultGameplayExperience;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectM")
	FGameplayTag WorldTag;
};