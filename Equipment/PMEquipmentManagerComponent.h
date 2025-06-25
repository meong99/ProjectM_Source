#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/PMAbilitySet.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "MEquipmentItemList.h"
#include "Types/MItemTypes.h"
#include "GameFramework/Pawn.h"
#include "Components/MPawnComponentBase.h"
#include "Components/MPlayerStateComponentBase.h"

#include "PMEquipmentManagerComponent.generated.h"

class UMEquipmentItemDefinition;
class UMEquipmentItemInstance;
class UPMAbilitySystemComponent;
class UPMAbilitySet;
class APlayerController;

/**
 *
 */
UCLASS(BlueprintType)
class PROJECTM_API UPMEquipmentManagerComponent : public UMPlayerStateComponentBase
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UPMEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void OnSetNewPawn(APawn* NewPawn, APawn* OldPawn) override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

/*
* Member Functions
*/
public:
	void EquipItem(const int32 ItemRowId);
	void UnequipItem(EMEquipmentItemType EquipmentItemType);
	bool IsEquipped(EMEquipmentItemType EquipmentItemType) { return FindEntry(EquipmentItemType) != nullptr; }

protected:
	UMEquipmentItemInstance*	FindEquippedItemInstance(EMEquipmentItemType EquipmentItemType);
	FMEquipmentItemEntry*		FindEntry(EMEquipmentItemType EquipmentItemType);

	void EquipItem_Impl(const UMEquipmentItemDefinition* EquipDef);
	void RemoveEntry(EMEquipmentItemType EquipmentItemType);

	void GiveAbilities(const UMEquipmentItemDefinition* EquipDef, UMEquipmentItemInstance* Instance);
	void TakeAbilitiesFromAsc(UMEquipmentItemInstance* Instance);

	void EquipAllItems();
	void UnequipAllItems();
	void EquipDefaultWeapon();

	UPMAbilitySystemComponent* GetAbilitySystemComponent() const;
/*
* Member Variables
*/
private:
	UPROPERTY(Replicated)
	FMEquipmentItemList EquippedItemList;

	UPROPERTY()
	TMap<int32, FMAbilitySet_GrantedHandles> GrantedHandles;

	UPROPERTY()
	TMap<EMEquipmentItemType, FMItemHandle> EquippedEntryMap;

	bool bIsInitialized = false;
};
