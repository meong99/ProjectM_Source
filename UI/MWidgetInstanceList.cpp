#include "MWidgetInstanceList.h"
#include "MWidgetBase.h"
#include "WidgetRegister/MWidgetRegister.h"

UMWidgetInstanceList::UMWidgetInstanceList(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{}

UMWidgetInstanceList* UMWidgetInstanceList::CreateNewWidgetInstanceList(UObject* WorldContextObject, const FGameplayTag& RegisterTag)
{
	if (!RegisterTag.IsValid())
	{
		MCHAE_WARNING("RegisterTag is not valid");
		return nullptr;
	}

	if (WorldContextObject == nullptr)
	{
		MCHAE_WARNING("WorldContext is null");
		return nullptr;
	}

	UMWidgetInstanceList* NewWidgetInstanceList = NewObject<UMWidgetInstanceList>(WorldContextObject);
	NewWidgetInstanceList->RegisterTag = RegisterTag;

	return NewWidgetInstanceList;
}

void UMWidgetInstanceList::CreateNewWidgets(UMWidgetRegister* WidgetRegister)
{
	if (WidgetRegister == nullptr)
	{
		MCHAE_WARNING("WidgetRegister is null!!");
		return;
	}

	const TArray<FFullLoadedWidgetData>& FullLoadedWidgetData = WidgetRegister->LoadAllWidgetAndGetData();
	for (FFullLoadedWidgetData LoadedWidgetData : FullLoadedWidgetData)
	{
		CreateNewWidget(LoadedWidgetData.WidgetTag, LoadedWidgetData.WidgetClass);
	}
}

UMWidgetBase* UMWidgetInstanceList::CreateNewWidget(const FGameplayTag& InWidgetTag, TSubclassOf<UMWidgetBase> WidgetClass)
{
	UMWidgetBase* NewWidget = WidgetInstanceMapWrapper.WidgetInstanceMap.FindRef(InWidgetTag);
	// 이미 있는지 확인
	if (NewWidget)
	{
		return NewWidget;
	}

	if (WidgetClass == nullptr)
	{
		MCHAE_WARNING("WidgetClass is null!");
		return nullptr;
	}

	NewWidget = CreateWidget<UMWidgetBase>(GetWorld(), WidgetClass);

	if (NewWidget)
	{
		NewWidget->SetWidgetTag(InWidgetTag);
		WidgetInstanceMapWrapper.WidgetInstanceMap.Emplace(InWidgetTag, NewWidget);
	}

	return NewWidget;
}

UMWidgetBase* UMWidgetInstanceList::GetWidgetInstance(const FGameplayTag& WidgetTag) const
{
	return WidgetInstanceMapWrapper.WidgetInstanceMap.FindRef(WidgetTag);
}
