#include "PMCameraComponent.h"
#include "PMCameraMode.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

UPMCameraComponent::UPMCameraComponent()
{
}

void UPMCameraComponent::OnRegister()
{
    Super::OnRegister();

    if (!CameraModeStack)
    {
        CameraModeStack = NewObject<UPMCameraModeStack>(this);
    }
}

void UPMCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
    check(CameraModeStack);

    UpdateCameraMode();

    FPMCameraModeView CameraModeView;
    CameraModeStack->EvaluateStack(DeltaTime, CameraModeView);

	if (APawn* TargetPawn = Cast<APawn>(GetTargetActor()))
	{
		if (APlayerController* PlayerController = TargetPawn->GetController<APlayerController>())
		{
			PlayerController->SetControlRotation(CameraModeView.ControlRotation);
		}
	}

	SetWorldLocationAndRotation(CameraModeView.Location, CameraModeView.Rotation);

	FieldOfView = CameraModeView.FieldOfView;

	DesiredView.Location = CameraModeView.Location;
	DesiredView.Rotation = CameraModeView.Rotation;
	DesiredView.FOV = CameraModeView.FieldOfView;
	DesiredView.OrthoWidth = OrthoWidth;
	DesiredView.OrthoNearClipPlane = OrthoNearClipPlane;
	DesiredView.OrthoFarClipPlane = OrthoFarClipPlane;
	DesiredView.AspectRatio = AspectRatio;
	DesiredView.bConstrainAspectRatio = bConstrainAspectRatio;
	DesiredView.bUseFieldOfViewForLOD = bUseFieldOfViewForLOD;
	DesiredView.ProjectionMode = ProjectionMode;
	DesiredView.PostProcessBlendWeight = PostProcessBlendWeight;
	if (PostProcessBlendWeight > 0.0f)
	{
		DesiredView.PostProcessSettings = PostProcessSettings;
	}
}

void UPMCameraComponent::UpdateCameraMode()
{
    check(CameraModeStack);

    if (DetermineCameraModeDelegate.IsBound())
    {
        if (const TSubclassOf<UPMCameraMode> CameraMode = DetermineCameraModeDelegate.Execute())
        {
            CameraModeStack->PushCameraMode(CameraMode);
        }
    }
}
