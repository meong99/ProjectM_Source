#include "MInventoryToastWidget.h"
#include "GameModes/PMExperienceManagerComponent.h"
#include "Inventory/PMInventoryManagerComponent.h"
#include "Character/Components/MWalletComponent.h"
#include "GameModes/PMExperienceDefinition.h"
#include "Inventory/MInventoryTypes.h"
#include "UI/Text/MFadeOutTextWidget.h"
#include "Components/VerticalBox.h"
#include "System/MDataTableManager.h"

UMInventoryToastWidget::UMInventoryToastWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMInventoryToastWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	UPMExperienceManagerComponent* ExperienceManager = GetWorld()->GetGameState()->FindComponentByClass<UPMExperienceManagerComponent>();
	if (ExperienceManager)
	{
		ExperienceManager->CallOrRegister_OnExperienceLoaded(FOnExperienceLoaded::FDelegate::CreateUObject(this, &UMInventoryToastWidget::OnExperienceLoaded));
	}

	TArray<UWidget*> Children = TextVerticalBox->GetAllChildren();
	for (int32 i = 0; i < Children.Num(); i++)
	{
		UMFadeOutTextWidget* Widget = Cast<UMFadeOutTextWidget>(Children[i]);
		if (Widget)
		{
			PushQue(Widget);
		}
	}
}

void UMInventoryToastWidget::InitThisWidget()
{
	APlayerController* Controller = GetOwningPlayer();
	InvenManager = Controller ? Controller->FindComponentByClass<UPMInventoryManagerComponent>() : nullptr;
	if (InvenManager)
	{
		InvenManager->Delegate_OnItemIncreased.AddUObject(this, &UMInventoryToastWidget::OnNotifyNewItem);
	}
	WalletComponent = Controller ? Controller->FindComponentByClass<UMWalletComponent>() : nullptr;
	if (WalletComponent)
	{
		WalletComponent->Delegate_OnChangeGold.AddDynamic(this, &UMInventoryToastWidget::OnChangeGold);
	}
}

void UMInventoryToastWidget::OnChangeGold(int64 AdjustPrice, int64 NewGold)
{
	if (AdjustPrice > 0.f)
	{
		UMFadeOutTextWidget* Widget = GetLastUsedWidget();
		if (Widget)
		{
			FText Template = FText::FromString("Gold (+{Value})");
			FFormatNamedArguments Args;
			Args.Add("Value", FText::AsNumber(AdjustPrice));
			FText NewText = FText::Format(Template, Args);

			Widget->PlayText(NewText);
		}
	}
}

void UMInventoryToastWidget::OnNotifyNewItem(const FMItemResponse& ItemRespons)
{
	if (ItemRespons.ItemRequest.RequestType == EMItemRequestType::AddItem)
	{
		UPMInventoryItemDefinition* ItemDef = UMDataTableManager::GetDefinitionObject<UPMInventoryItemDefinition>(this, ItemRespons.ItemRequest.ItemRowId);
		if (ItemDef)
		{
			UMFadeOutTextWidget* Widget = GetLastUsedWidget();
			if (Widget)
			{
				FText Template = FText::FromString("New Item!!  ({Value})");
				FFormatNamedArguments Args;
				Args.Add("Value", ItemDef->DisplayName);
				FText NewText = FText::Format(Template, Args);

				Widget->PlayText(NewText);
			}
		}
	}
}

void UMInventoryToastWidget::OnExperienceLoaded(const UPMExperienceDefinition* LoadedExperienceDefinition)
{
	InitThisWidget();
}

UMFadeOutTextWidget* UMInventoryToastWidget::GetLastUsedWidget()
{
	UMFadeOutTextWidget* Widget = PopQue();
	PushQue(Widget);
	return Widget;
}

void UMInventoryToastWidget::PushQue(UMFadeOutTextWidget* Widget)
{
	if (Widget)
	{
		ActivateWidgetQue.Enqueue(Widget);
		++QueSize;
	}
}

UMFadeOutTextWidget* UMInventoryToastWidget::PopQue()
{
	UMFadeOutTextWidget* Widget = nullptr;
	if (!ActivateWidgetQue.IsEmpty())
	{
		ActivateWidgetQue.Peek(Widget);
		ActivateWidgetQue.Pop();
		--QueSize;
	}

	return Widget;
}
