#include "PMCombatSet.h"
#include "Net/UnrealNetwork.h"

UPMCombatSet::UPMCombatSet() : Super(), DefensePower(0.f), AttackPower(0.0f)
{
}

void UPMCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPMCombatSet, AttackPower);
	DOREPLIFETIME(UPMCombatSet, DefensePower);
}
