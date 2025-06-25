#include "MHealthRegenexecution.h"
#include "AbilitySystem/Attributes/PMHealthSet.h"

struct FHealthRecoveryStatics
{
	FGameplayEffectAttributeCaptureDefinition HealthRecovery;

	FHealthRecoveryStatics()
	{
		HealthRecovery = FGameplayEffectAttributeCaptureDefinition(UPMHealthSet::GetHealthRecoveryAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	}
};

static FHealthRecoveryStatics& HealthRecoveryStatics()
{
	static FHealthRecoveryStatics Statics;
	return Statics;
}

UMHealthRegenexecution::UMHealthRegenexecution()
{
	RelevantAttributesToCapture.Add(HealthRecoveryStatics().HealthRecovery);
}

void UMHealthRegenexecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	float HealthRecovery = 0.0f;
	{
		FAggregatorEvaluateParameters EvaluateParameters;

		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealthRecoveryStatics().HealthRecovery, EvaluateParameters, HealthRecovery);
	}

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UPMHealthSet::GetHealthAttribute(), EGameplayModOp::Additive, HealthRecovery));
}