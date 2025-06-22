#pragma once

#include "CoreMinimal.h"
#include "MInteractionWidgetBase.h"
#include "MInteractionKeyWidget.generated.h"

UCLASS()
class PROJECTM_API UMInteractionKeyWidget : public UMInteractionWidgetBase
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UMInteractionKeyWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;

	/*
* Member Functions
*/
public:

/*
* Member Variables
*/
protected:
};
