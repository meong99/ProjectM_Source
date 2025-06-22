#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Inventory/MInventoryTypes.h"
#include "Templates/SubclassOf.h"
#include "Definitions/MDefinitionBase.h"
#include "Types/MItemTypes.h"

#include "PMInventoryItemDefinition.generated.h"

class UPMInventoryItemInstance;
class UTexture2D;
class UGameplayEffect;
class UCurveTable;
class UMGameplayEffectSet;

/* Remind
* Item의 Component라고 이해하면 편하다.
* 각 Item은 개별적인 능력들이 존재하는데 그걸 아이템에 종속시키지 않고 컴포넌트처럼 별도로 생성 후 아이템에 붙이는 형태로 간다.
* 그렇게 하면 아이템의 능력들이 아이템에 종속되지 않아서 필요한 능력들을 원하는 대로 붙여서 새로운 아이템을 쉽게 만들어낼 수 있다.
*/
#pragma NOTE("DefaultToInstanced, EditInlineNew, Instanced를 조합해서 BP CDO를 조작할 수 있다는 것. 생각해둬야한다.")
UCLASS(Abstract, DefaultToInstanced, EditInlineNew)
class UPMInventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UPMInventoryItemInstance* Instance) const {};
};

/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class PROJECTM_API UPMInventoryItemDefinition : public UMDefinitionBase
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UPMInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

/*
* Member Functions
*/
public:
	const UPMInventoryItemFragment* FindFragmentByClass(TSubclassOf<UPMInventoryItemFragment> FragmentClass) const;

	const TArray<TObjectPtr<UPMInventoryItemFragment>>&	GetFragments() const { return Fragments; }
/*
* Member Variables
*/
public:
	//아이템의 고유 Id
	UPROPERTY(BlueprintReadOnly, Category = "Item")
	int32 ItemId = INDEX_NONE;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	EMItemType ItemType = EMItemType::None;

	//아이템의 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	UTexture2D* ItemIcon;
	
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	TSubclassOf<UPMInventoryItemInstance> InstanceType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Item")
	TArray<TObjectPtr<UMGameplayEffectSet>> EffectSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	int32 SellPrice = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	int32 BuyPrice = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(MultiLine = true), Category = "Item")
	FText ItemContext;

	// 아이템의 컴포넌트라고 생각할 수 있다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Item")
	TArray<TObjectPtr<UPMInventoryItemFragment>> Fragments;
};