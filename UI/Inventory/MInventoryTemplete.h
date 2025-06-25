// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MWidgetBase.h"
#include "MInventoryTemplete.generated.h"

class UMTileView;

UCLASS()
class PROJECTM_API UMInventoryTemplete : public UMWidgetBase
{
	GENERATED_BODY()

	/*
	* Overrided Functions
	*/
public:
	UMInventoryTemplete(const FObjectInitializer& ObjectInitializer);

/*
* Member Functions
*/
public:
	UMTileView* GetTileView() const { return TileView; }
/*
* Member Variables
*/
protected:
	UPROPERTY(meta = (BindWidget))
	UMTileView* TileView;
};
