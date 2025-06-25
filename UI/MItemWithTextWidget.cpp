#include "MItemWithTextWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Item/MContextableItemWidget.h"
#include "System/MDataTableManager.h"

UMItemWithTextWidget::UMItemWithTextWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMItemWithTextWidget::SetItem(UPMInventoryItemDefinition* ItemDef)
{
	if (ItemDef)
	{
		ItemImage->SetItemRowId(ItemDef->RowId);
		ItemImage->SetBrushFromTexture(ItemDef->ItemIcon);
	}
}

void UMItemWithTextWidget::SetItemContextText(const FText& ItemContext)
{
	ItemContextText->SetText(ItemContext);
}
