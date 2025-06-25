#pragma once

#include "PMCameraMode.h"
#include "PMCameraMode_ThirdPerson.generated.h"

class UCurveVector;

UCLASS(Abstract, Blueprintable)
class PROJECTM_API UPMCameraMode_ThirdPerson : public UPMCameraMode
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UPMCameraMode_ThirdPerson();

/*
* Member Functions
*/
public:

private:
	virtual void UpdateView(float DeltaTime) override;

/*
* Member Variables
*/
private:
	UPROPERTY(EditDefaultsOnly, Category = "ProjectDD | ThirdPerson")
	TObjectPtr<const UCurveVector> TargetOffsetCurve;
};