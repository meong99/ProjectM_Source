#pragma once

#include "CoreMinimal.h"
#include "PMPlayerCameraManager.h"
#include "PMCameraMode.generated.h"

class UPMCameraComponent;

struct FPMCameraModeView
{
	FPMCameraModeView()
	: Location(ForceInit)
	, Rotation(ForceInit)
	, ControlRotation(ForceInit)
	, FieldOfView(CAMERA_DEFAULT_FOV)
	{}

	void Blend(const FPMCameraModeView& Other, float OtherWeight);

	FVector		Location;
	FRotator	Rotation;
	FRotator	ControlRotation;
	float		FieldOfView;
};

UENUM(BlueprintType)
enum class EPMCamaraModeBlendFunction : uint8
{
	Linear,
	EaseIn,
	EaseOut,
	EaseInOut,

	COUNT
};

UCLASS(Abstract, NotBlueprintable)
class PROJECTM_API UPMCameraMode : public UObject
{
	GENERATED_BODY()

	friend UPMCameraModeStack;
/*
* Overrided Function
*/
public:
	UPMCameraMode();

/*
* Member Functions
*/
public:
	void UpdateCameraMode(float DeltaTime);

protected:
	virtual void	UpdateView(float DeltaTime);
	void			UpdateBlending(float DeltaTime);

	FVector				GetPivotLocation() const;
	FRotator			GetPivotRotation() const;
	AActor*				GetTargetActor() const;
	UPMCameraComponent*	GetCameraComponent() const;
/*
* Member Variables
*/
protected:
	FPMCameraModeView View;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectDD | View", Meta = (UIMin = "5.0", UIMax = "170.0", ClampMin = "5.0", ClampMax = "170.0"))
	float FieldOfView;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectDD | View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMin;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectDD | View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMax;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectDD | Blending")
	float BlendTime;

	float BlendAlpha;

	float BlendWeight;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectCR | Blending")
	float BlendExponent;

	EPMCamaraModeBlendFunction BlendFunction;
};

UCLASS()
class UPMCameraModeStack : public UObject
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UPMCameraModeStack();

/*
* Member Functions
*/
public:
	UPMCameraMode* GetCameraModeInstance(const TSubclassOf<UPMCameraMode>& CameraModeClass);
	void PushCameraMode(const TSubclassOf<UPMCameraMode>& CameraModeClass);
	void EvaluateStack(float DeltaTime, FPMCameraModeView& OutCameraModeView);

private:
	void UpdateStack(float DeltaTime);
	void BlendStack(FPMCameraModeView& OutCameraModeView);
/*
* Member Variables
*/
private:
	// 카메라 모드 오브젝트 풀링 용도. 캐싱해뒀다가 필요할 때 마다 꺼내서 쓴다.
	UPROPERTY()
	TArray<TObjectPtr<UPMCameraMode>> CameraModeInstances;

	// 실제로 사용중인 카메라 모드들이다.
	UPROPERTY()
	TArray<TObjectPtr<UPMCameraMode>> CameraModeStack;
};