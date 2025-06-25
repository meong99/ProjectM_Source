#include "PMPlayerCameraManager.h"

APMPlayerCameraManager::APMPlayerCameraManager(const FObjectInitializer& ObjectInitializer)
{
	DefaultFOV = CAMERA_DEFAULT_FOV;
	ViewPitchMin = CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = CAMERA_DEFAULT_PITCH_MAX;
}
// 
// void APMPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
// {
// }
// 
// void APMPlayerCameraManager::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos)
// {
// }
