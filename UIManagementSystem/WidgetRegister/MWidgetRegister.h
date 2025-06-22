#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "MWidgetRegister.generated.h"

class UMWidgetBase;

USTRUCT(BlueprintType)
struct FMappedWidgetData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta = (Categories = "UI.Registry"))
	TMap<FGameplayTag, TSoftClassPtr<UMWidgetBase>> WidgetData;
};

USTRUCT(BlueprintType)
struct FFullLoadedWidgetData
{
	GENERATED_BODY()

	FFullLoadedWidgetData() {};
	FFullLoadedWidgetData(const FGameplayTag& InWidgetTag, TSubclassOf<UMWidgetBase> InWidgetClass)
	{
		WidgetTag = InWidgetTag;
		WidgetClass = InWidgetClass;
	}

	FGameplayTag WidgetTag;

	UPROPERTY()
	TSubclassOf<UMWidgetBase> WidgetClass;
};

/*
* 인게임에서 사용할 위젯을 등록
*/
UCLASS()
class PROJECTM_API UMWidgetRegister : public UPrimaryDataAsset
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UMWidgetRegister();

/*
* Member Functions
*/
public:
	TSubclassOf<UMWidgetBase>		GetWidgetClass(const FGameplayTag& Tag) const;
	TArray<FFullLoadedWidgetData> 	LoadAllWidgetAndGetData();

private:
	TSubclassOf<UMWidgetBase>	GetWidgetClass_Impl(const TSoftClassPtr<UMWidgetBase>& SoftClass) const;

/*
* Member Variables
*/
public:
	// WidgetRegister들을 구분지을 태그. 특정 위젯군을 찾을 때 사용한다.
	UPROPERTY(EditDefaultsOnly, Category = "WidgetRegister", meta = (Categories="UI.Registry"))
	FGameplayTag RegisterTag;

	// Widget Metadata
	UPROPERTY(EditDefaultsOnly, Category = "WidgetRegister")
	FMappedWidgetData MappedWidgetData;

	UPROPERTY()
	const UObject* WorldContext;
};
