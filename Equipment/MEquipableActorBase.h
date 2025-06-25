#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/Equipment/MEquipmentItemDefinition.h"
#include "MEquipableActorBase.generated.h"

/**
 *
 */
UCLASS()
class PROJECTM_API AMEquipableActorBase : public AActor
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	AMEquipableActorBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

/*
* Member Functions
*/
public:
	void SetItemDef(TSubclassOf<UMEquipmentItemDefinition> InItemDef) { ItemDef = InItemDef; }

/*
* Member Variables
*/
protected:
	TSubclassOf<UMEquipmentItemDefinition> ItemDef;
};