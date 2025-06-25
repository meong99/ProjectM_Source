#include "MWidgetRegister.h"
#include "Blueprint/UserWidget.h"
#include "UI/MWidgetBase.h"

UMWidgetRegister::UMWidgetRegister()
{
}

TSubclassOf<UMWidgetBase> UMWidgetRegister::GetWidgetClass(const FGameplayTag& Tag) const
{
	const TSoftClassPtr<UMWidgetBase>& WidgetSoftClass = MappedWidgetData.WidgetData.FindRef(Tag);
	
	return GetWidgetClass_Impl(WidgetSoftClass);
}

TArray<FFullLoadedWidgetData> UMWidgetRegister::LoadAllWidgetAndGetData()
{
	TArray<FFullLoadedWidgetData> LoadedWidgetData;

	for (const auto& Iter : MappedWidgetData.WidgetData)
	{
		TSubclassOf<UMWidgetBase> WidgetClass = GetWidgetClass_Impl(Iter.Value);
		if (WidgetClass.Get())
		{
			LoadedWidgetData.Emplace(Iter.Key, WidgetClass);
		}
		else
		{
			MCHAE_WARNING("Something worng with widget class loading. Can't get widget class pointer. KeyTag = %s", *Iter.Key.ToString());
		}
	}

	return LoadedWidgetData;
}

TSubclassOf<UMWidgetBase> UMWidgetRegister::GetWidgetClass_Impl(const TSoftClassPtr<UMWidgetBase>& SoftClass) const
{
	TSubclassOf<UMWidgetBase> WidgetClass = SoftClass.Get();
	if (!WidgetClass)
	{
		WidgetClass = SoftClass.LoadSynchronous();
	}

	if (!WidgetClass)
	{
		MCHAE_WARNING("It's only setted key! check widget soft class in widget register!");
	}

	return WidgetClass;
}
