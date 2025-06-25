#include "MNameWidget.h"
#include "Components/TextBlock.h"

UE_DISABLE_OPTIMIZATION
UMNameWidget::UMNameWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMNameWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UMNameWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UMNameWidget::SetName(const FName& NewName)
{
	if (Name && !NewName.IsNone())
	{
		Name->SetText(FText::FromName(NewName));
	}
}

FVector2D UMNameWidget::GetSize() const
{
	return Name->GetDesiredSize();
}
UE_ENABLE_OPTIMIZATION
