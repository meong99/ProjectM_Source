#pragma once

#include "CoreMinimal.h"
#include "Inventory/PMInventoryItemDefinition.h"
#include "Types/MItemTypes.h"
#include "MEquipmentItemDefinition.generated.h"

class UPMAbilitySet;
class UMEquipableItemInstance;
class UMEquipmentItemInstance;

USTRUCT()
struct FPMEquipmentActorToSpawn
{
	GENERATED_BODY()

	// Spawn할 Actor의 클래스. 기능을 가진다기 보다는 실제 월드에 소환되는 클래스
	UPROPERTY(EditAnywhere, Category = "Equipment")
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category = "Equipment")
	FName AttachSocket;

	UPROPERTY(EditAnywhere, Category = "Equipment")
	FTransform AttachTransform;
};

UCLASS()
class PROJECTM_API UMEquipmentItemDefinition : public UPMInventoryItemDefinition
{
	GENERATED_BODY()
/*
* Overrided Functions
*/
public:
	UMEquipmentItemDefinition();

/*
* Member Functions
*/
public:

/*
* Member Variables
*/
public:
	// 장비 장착 시 사용될 Instance의 Type
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TSubclassOf<UMEquipmentItemInstance> EquipmentInstanceType;

	// Spawn될 장비에 대한 메타데이터
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	FPMEquipmentActorToSpawn ActorToSpawn;

	// 장비 장착으로 부여될 어빌리티
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TArray<TObjectPtr<UPMAbilitySet>> AbilitySetsToGrant;

	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	EMEquipmentItemType EquipmentItemType;
};