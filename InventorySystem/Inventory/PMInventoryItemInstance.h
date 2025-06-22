#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Templates/SubclassOf.h"
#include "PMInventoryItemDefinition.h"
#include "System/PMGameplayTagStack.h"

#include "PMInventoryItemInstance.generated.h"

/**
 *
 */
UCLASS(BlueprintType)
class PROJECTM_API UPMInventoryItemInstance : public UObject
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UPMInventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual bool IsSupportedForNetworking() const override { return true; }
/*
* Member Functions
*/
public:
	UFUNCTION(BlueprintCallable, BlueprintPure = false, meta = (DeterminesOutputType = FragmentClass))
	const UPMInventoryItemFragment* FindFragmentByClass(TSubclassOf<UPMInventoryItemFragment> FragmentClass) const;

	template<typename ResultClass>
	const ResultClass* FindFragmentByClass() const
	{
		return (ResultClass*)FindFragmentByClass(ResultClass::StaticClass());
	}

	virtual void OnInstanceCreated() {};

	virtual bool ActivateItem();
	virtual bool CanUseItem() const { return false; }

	/** add/remove stack count to stat tag(=gameplay-tag stack) */
	int32 AddStatTagStack(FGameplayTag Tag, int32 StackCount);
	int32 RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

	/** whether stat tag has in StatTags */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool HasStatTag(FGameplayTag Tag) const;

	/** get the current count of gameplay-tag stack */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	int32 GetStatTagStackCount(FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	int32 GetItemQuentity() const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	EMItemType GetItemType() const;

/*
* Member Variables
*/
public:
	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 ItemRowId = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Replicated)
	TSubclassOf<UPMInventoryItemDefinition> ItemDef;

	/** gameplay-tag stacks for inventory item instance */
	UPROPERTY(BlueprintReadOnly, Replicated)
	FPMGameplayTagStackContainer StatTags;

	UPROPERTY(BlueprintReadOnly, Replicated)
	FMItemHandle ItemHandle;

	UPROPERTY(BlueprintReadOnly, Replicated)
	FMItemResponse ItemResponse;
};