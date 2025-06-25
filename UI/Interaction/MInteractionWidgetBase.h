#pragma once

#include "CoreMinimal.h"
#include "UI/MWorldBaseWidget.h"
#include "MInteractionWidgetBase.generated.h"

class AActor;

UCLASS()
class PROJECTM_API UMInteractionWidgetBase : public UMWorldBaseWidget
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UMInteractionWidgetBase(const FObjectInitializer& ObjectInitializer);

/*
* Member Functions
*/
public:
/*
* Member Variables
*/
protected:
};
