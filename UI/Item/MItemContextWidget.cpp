#include "MItemContextWidget.h"
#include "Components/Image.h"
#include "System/MDataTableManager.h"
#include "Inventory/PMInventoryItemDefinition.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "PMGameplayTags.h"
#include "AbilitySystem/MGameplayEffectSet.h"
UMItemContextWidget::UMItemContextWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMItemContextWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMItemContextWidget::UpdateContextWidget(const int32 RowId)
{
	ItemRowId = RowId;
	UPMInventoryItemDefinition* ItemDef = UMDataTableManager::GetDefinitionObject<UPMInventoryItemDefinition>(this, ItemRowId);
	DisableTexts();
	if (ItemDef)
	{
		SetItemImage(ItemDef->ItemIcon);
		SetItemContextText(ItemDef->ItemContext);
		SetItemName(ItemDef->DisplayName);

		bool bExistStat = false;
		for (const UMGameplayEffectSet* EffectSet : ItemDef->EffectSet)
		{
			if (!EffectSet)
			{
				continue;
			}
			const TArray<FMAbilitySet_GameplayEffect>& EffectsToApply = EffectSet->GetEffectToApply();

			for (const FMAbilitySet_GameplayEffect& EffectInfo : EffectsToApply)
			{
				for (const FMSetbyCallerFloat& SetbyCallerInfo : EffectInfo.SetbyCallers)
				{
					UTextBlock* OutTextBlock = nullptr;
					FText OutName;
					GetTextBlockByTag(SetbyCallerInfo.SetByCallerTag, OutTextBlock, OutName);
					if (OutTextBlock && SetbyCallerInfo.Value > 0.f)
					{
						SetSpecText(OutTextBlock, OutName, SetbyCallerInfo.Value);
						bExistStat = true;
					}
				}
			}
		}

		bExistStat ? StatOverlay->SetVisibility(ESlateVisibility::SelfHitTestInvisible) : StatOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}

	SetWidgetPosition();
}

void UMItemContextWidget::SetWidgetPosition()
{
	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(FloatingTarget->Slot);
	if (CanvasSlot)
	{
		const float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(GetWorld());
		FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(this) / ViewportScale;
		FloatingTarget->ForceLayoutPrepass();
		FVector2D WidgetSize = FloatingTarget->GetDesiredSize();
		FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
		FVector2D WidgetPosition = MousePosition;

		if (WidgetPosition.X < 0)
		{
			WidgetPosition.X = 0;
		}
		else if (WidgetPosition.X + WidgetSize.X > ViewportSize.X)
		{
			WidgetPosition.X = MousePosition.X - WidgetSize.X;
		}
		if (WidgetPosition.Y < 0)
		{
			WidgetPosition.Y = 0;
		}
		else if (WidgetPosition.Y + WidgetSize.Y > ViewportSize.Y)
		{
			WidgetPosition.Y = MousePosition.Y - WidgetSize.Y;
		}

		CanvasSlot->SetPosition(WidgetPosition);
	}
}

void UMItemContextWidget::DisableTexts()
{
	Health->SetVisibility(ESlateVisibility::Collapsed);
	MaxHealth->SetVisibility(ESlateVisibility::Collapsed);
	AttackPower->SetVisibility(ESlateVisibility::Collapsed);
	DefencePower->SetVisibility(ESlateVisibility::Collapsed);
	HealthRecovery->SetVisibility(ESlateVisibility::Collapsed);
}

void UMItemContextWidget::SetItemImage(UTexture2D* NewTexture)
{
	ItemImage->SetBrushFromTexture(NewTexture);
}

void UMItemContextWidget::SetItemContextText(const FText& NewContext)
{
	ItemContext->SetText(NewContext);
}

void UMItemContextWidget::SetItemName(const FText& NewName)
{
	ItemName->SetText(NewName);
}

void UMItemContextWidget::SetSpecText(UTextBlock* InTextBlock, const FText& InName, const float Value)
{
	FText Template = FText::FromString("{Name} : {Value}");
	FFormatNamedArguments Args;

	Args.Add("Value", FText::AsNumber(Value));
	Args.Add("Name", InName);

	if (InTextBlock)
	{
		InTextBlock->SetText(FText::Format(Template, Args));
		InTextBlock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UMItemContextWidget::GetTextBlockByTag(const FGameplayTag& AttributeTag, UTextBlock*& OutTextBlock, FText& OutName)
{
	if (AttributeTag == FPMGameplayTags::Get().Ability_Effect_SetByCaller_Health)
	{
		OutTextBlock = Health;
		OutName = FText::FromString(TEXT("회복량"));
	}
	else if (AttributeTag == FPMGameplayTags::Get().Ability_Effect_SetByCaller_MaxHealth)
	{
		OutTextBlock = MaxHealth;
		OutName = FText::FromString(TEXT("최대 체력"));
	}
	else if (AttributeTag == FPMGameplayTags::Get().Ability_Effect_SetByCaller_AttackPower)
	{
		OutTextBlock = AttackPower;
		OutName = FText::FromString(TEXT("공격력"));
	}
	else if (AttributeTag == FPMGameplayTags::Get().Ability_Effect_SetByCaller_DefensePower)
	{
		OutTextBlock = DefencePower;
		OutName = FText::FromString(TEXT("방어력"));
	}
	else if (AttributeTag == FPMGameplayTags::Get().Ability_Effect_SetByCaller_HealthRecovery)
	{
		OutTextBlock = HealthRecovery;
		OutName = FText::FromString(TEXT("초당 체력 회복"));
	}
	else
	{
		OutTextBlock = nullptr;
	}
}
