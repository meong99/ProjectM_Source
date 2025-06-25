#pragma once

#include "CoreMinimal.h"
#include "UI/MWidgetBase.h"
#include "MOptionWidget.generated.h"

class UButton;

UCLASS()
class PROJECTM_API UMOptionWidget : public UMWidgetBase
{
	GENERATED_BODY()

	/*
	* Overrided Functions
	*/
public:
	UMOptionWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeOnInitialized() override;

	/*
	* Member Functions
	*/
private:
	void BindDelegates();

	UFUNCTION()
	void OnClick_ExitButton();
	UFUNCTION()
	void OnClick_LobbyButton();
	
	/*
	* Member Variables
	*/
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UButton> ExitButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "ProjectM")
	TObjectPtr<UButton> LobbyButton;
};
