#pragma once

#include "CommonGameInstance.h"
#include "PMGameInstance.generated.h"

UCLASS()
class PROJECTM_API UPMGameInstance : public UCommonGameInstance
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UPMGameInstance();

	virtual void Init() override;
	virtual void Shutdown() override;

/*
* Member Functions
*/
public:

/*
* Member Variables
*/
public:
};