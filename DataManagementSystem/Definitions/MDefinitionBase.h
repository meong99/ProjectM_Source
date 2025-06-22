#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Templates/SubclassOf.h"

#include "MDefinitionBase.generated.h"

/**
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class PROJECTM_API UMDefinitionBase : public UObject
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UMDefinitionBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

/*
* Member Functions
*/
public:
	UFUNCTION(BlueprintCallable)
	int32 GetRowId() const { return RowId; }
/*
* Member Variables
*/
public:
	UPROPERTY(VisibleAnywhere)
	int32 RowId = INDEX_NONE;
};
