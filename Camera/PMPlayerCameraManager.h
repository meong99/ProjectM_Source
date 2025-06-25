// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Camera/PlayerCameraManager.h"

#include "PMPlayerCameraManager.generated.h"

class FDebugDisplayInfo;
class UCanvas;
class UObject;

#define CAMERA_DEFAULT_FOV			(80.0f)
#define CAMERA_DEFAULT_PITCH_MIN	(-89.0f)
#define CAMERA_DEFAULT_PITCH_MAX	(89.0f)

UCLASS(notplaceable, MinimalAPI)
class APMPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:

	APMPlayerCameraManager(const FObjectInitializer& ObjectInitializer);

protected:

// 	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
// 
// 	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;
};
