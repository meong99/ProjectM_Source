#include "PMCameraMode.h"
#include "GameFramework/Pawn.h"
#include "PMCameraComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(PMCameraMode)

/*
* FPMCameraModeView
*/
void FPMCameraModeView::Blend(const FPMCameraModeView& Other, float OtherWeight)
{
	if (OtherWeight <= 0.0f)
	{
		return;
	}
	if (OtherWeight >= 1.0f)
	{
		*this = Other;
		return;
	}

	Location = FMath::Lerp(Location, Other.Location, OtherWeight);

	const FRotator DeltaRotation = (Other.Rotation - Rotation).GetNormalized();
	Rotation = Rotation + (OtherWeight * DeltaRotation);

	const FRotator DeltaControlRotation = (Other.ControlRotation - ControlRotation).GetNormalized();
	ControlRotation = ControlRotation + (OtherWeight * DeltaControlRotation);

	FieldOfView = FMath::Lerp(FieldOfView, Other.FieldOfView, OtherWeight);
}

/*
* UPMCameraMode
*/
UPMCameraMode::UPMCameraMode()
{
	FieldOfView = CAMERA_DEFAULT_FOV;
	ViewPitchMin = CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = CAMERA_DEFAULT_PITCH_MAX;

	BlendTime = 0.0f;
	BlendAlpha = 1.0f;
	BlendWeight = 1.0f;

	BlendFunction = EPMCamaraModeBlendFunction::EaseOut;
	BlendExponent = 4.0f;
}

void UPMCameraMode::UpdateCameraMode(float DeltaTime)
{
	UpdateView(DeltaTime);
	UpdateBlending(DeltaTime);
}

void UPMCameraMode::UpdateView(float DeltaTime)
{
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = GetPivotRotation();

	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

	View.Location = PivotLocation;
	View.Rotation = PivotRotation;

	View.ControlRotation = View.Rotation;
	View.FieldOfView = FieldOfView;
}

void UPMCameraMode::UpdateBlending(float DeltaTime)
{
	if (BlendTime > 0.f)
	{
		BlendAlpha += DeltaTime / BlendTime;
	}
	else
	{
		BlendAlpha = 1.f;
	}
	const float Exponent = (BlendExponent > 0.0f) ? BlendExponent : 1.0f;

	switch (BlendFunction)
	{
		case EPMCamaraModeBlendFunction::Linear:
			BlendWeight = BlendAlpha;
			break;
		case EPMCamaraModeBlendFunction::EaseIn:
			BlendWeight = FMath::InterpEaseIn(0.0f, 1.0f, BlendAlpha, Exponent);
			break;
		case EPMCamaraModeBlendFunction::EaseOut:
			BlendWeight = FMath::InterpEaseOut(0.0f, 1.0f, BlendAlpha, Exponent);
			break;
		case EPMCamaraModeBlendFunction::EaseInOut:
			BlendWeight = FMath::InterpEaseInOut(0.0f, 1.0f, BlendAlpha, Exponent);
			break;
		default:
			checkf(false, TEXT("UpdateBlending: Invalid BlendFunction [%d]\n"), (uint8)BlendFunction);
			break;
	}
}

FVector UPMCameraMode::GetPivotLocation() const
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		return TargetPawn->GetPawnViewLocation();
	}

	return TargetActor->GetActorLocation();
}

FRotator UPMCameraMode::GetPivotRotation() const
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		return TargetPawn->GetViewRotation();
	}

	return TargetActor->GetActorRotation();
}

AActor* UPMCameraMode::GetTargetActor() const
{
	const UPMCameraComponent* CameraComponent = GetCameraComponent();
	return CameraComponent->GetTargetActor();
}

UPMCameraComponent* UPMCameraMode::GetCameraComponent() const
{
	return CastChecked<UPMCameraComponent>(GetOuter());
}

/*
* UPMCameraModeStack
*/
UPMCameraModeStack::UPMCameraModeStack()
{
}

UPMCameraMode* UPMCameraModeStack::GetCameraModeInstance(const TSubclassOf<UPMCameraMode>& CameraModeClass)
{
	check(CameraModeClass);

	for (UPMCameraMode* CameraMode : CameraModeInstances)
	{
		if (CameraMode != nullptr && CameraMode->GetClass() == CameraModeClass)
		{
			return CameraMode;
		}
	}

	UPMCameraMode* NewCameraMode = NewObject<UPMCameraMode>(GetOuter(), CameraModeClass, NAME_None, RF_NoFlags);
	check(NewCameraMode);

	CameraModeInstances.Add(NewCameraMode);

	return NewCameraMode;
}

void UPMCameraModeStack::PushCameraMode(const TSubclassOf<UPMCameraMode>& CameraModeClass)
{

	if (!CameraModeClass)
	{
		return;
	}

	UPMCameraMode* CameraMode = GetCameraModeInstance(CameraModeClass);
	check(CameraMode);

	// 가장 앞 단에 새로운 카메라 모드가 스태킹되어있다면 그냥 리턴
	int32 StackSize = CameraModeStack.Num();
	if (StackSize > 0 && CameraModeStack[0] == CameraMode)
	{
		return;
	}

	int32 ExistingStackIndex = INDEX_NONE;
	float ExistingStackContribution = 1.0f;


	/*
	* 가장 최근의 BlendWeight값을 0.0f, 예전 데이터가 1.0f로 정했을 때 각각 카메라모드의 비율을 계산해서 적용시킨다.
	* 각 비율을 모두 더했을 때 1.0f이 나오도록 해야한다.
	* 
	* BlendWeight	|	ExistingStackContribution				|	ExistingStackContribution (accumulated)
	* 0.1f			|	(1.0f)	 * 0.1f = 0.1f					|	(1.0f - 0.1f)		  = 0.9f
	* 0.3f			|	(0.9f)	 * 0.3f = 0.27f					|	(1.0f - 0.3f) * 0.9f  = 0.63f
	* 0.6f			|	(0.63f)  * 0.6f = 0.378f				|	(1.0f - 0.6f) * 0.63f = 0.252f
	* 1.0f			|	(0.252f) * 1.0f = 0.252f				|
	*				|											|
	*				|			↓↓↓↓↓↓↓↓↓↓						|
	*				|											|
	*				|	0.1f + 0.27f + 0.378f +  0.252f = 1.0f	|
	*/
	for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex) 
	{
		if (CameraModeStack[StackIndex] == CameraMode)
		{
			ExistingStackIndex = StackIndex;
			ExistingStackContribution *= CameraMode->BlendWeight;
			break;
		}
		else
		{
			ExistingStackContribution *= 1.0f - CameraModeStack[StackIndex]->BlendWeight;
		}
	}

	if (ExistingStackIndex != INDEX_NONE)
	{
		CameraModeStack.RemoveAt(ExistingStackIndex);
		StackSize--;
	}
	else
	{
		ExistingStackContribution = 0.0f;
	}

	const bool bShouldBlend = CameraMode->BlendTime > 0.f && StackSize > 0;
	const float BlendWeight = bShouldBlend ? ExistingStackContribution : 1.0f;
	CameraMode->BlendWeight = BlendWeight;

	CameraModeStack.Insert(CameraMode, 0);
	CameraModeStack.Last()->BlendWeight = 1.0f;
}

void UPMCameraModeStack::EvaluateStack(float DeltaTime, FPMCameraModeView& OutCameraModeView)
{
	// Stack[0 -> Num] 까지 순차적으로 CameraMode Update
	UpdateStack(DeltaTime);

	// Num -> 0까지 CameraModeStack에 대해 Blending진행
	BlendStack(OutCameraModeView);
}

void UPMCameraModeStack::UpdateStack(float DeltaTime)
{
	const int32 StackSize = CameraModeStack.Num();
	if (StackSize <= 0)
	{
		return;
	}

	int32 RemoveCount = 0;
	int32 RemoveIndex = INDEX_NONE;
	for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
	{
		UPMCameraMode* CameraMode = CameraModeStack[StackIndex];
		check(CameraMode);

		CameraMode->UpdateCameraMode(DeltaTime);

		if (CameraMode->BlendWeight >= 1.0f)
		{
			RemoveIndex = StackIndex + 1;
			RemoveCount = StackSize - RemoveIndex;
			break;
		}
	}

	if (RemoveCount > 0)
	{
		CameraModeStack.RemoveAt(RemoveIndex, RemoveCount);
	}
}

void UPMCameraModeStack::BlendStack(FPMCameraModeView& OutCameraModeView)
{
	const int32 StackSize = CameraModeStack.Num();
	if (StackSize <= 0)
	{
		return;
	}

	const UPMCameraMode* CameraMode = CameraModeStack[StackSize - 1];
	check(CameraMode);

	OutCameraModeView = CameraMode->View;

	for (int32 StackIndex = StackSize - 2; StackIndex >= 0; --StackIndex)
	{
		CameraMode = CameraModeStack[StackIndex];
		check(CameraMode);

		OutCameraModeView.Blend(CameraMode->View, CameraMode->BlendWeight);
	}
}
 