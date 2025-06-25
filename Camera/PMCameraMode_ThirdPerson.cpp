#include "PMCameraMode_ThirdPerson.h"
#include "Curves/CurveVector.h"

UPMCameraMode_ThirdPerson::UPMCameraMode_ThirdPerson()
{
}

void UPMCameraMode_ThirdPerson::UpdateView(float DeltaTime)
{
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = GetPivotRotation();

	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

	View.Location = PivotLocation;
	View.Rotation = PivotRotation;
	View.ControlRotation = View.Rotation;
	View.FieldOfView = FieldOfView;

	if (TargetOffsetCurve)
	{
		const FVector Targetoffset = TargetOffsetCurve->GetVectorValue(PivotRotation.Pitch);
		View.Location = PivotLocation + PivotRotation.RotateVector(Targetoffset);
	}
}
