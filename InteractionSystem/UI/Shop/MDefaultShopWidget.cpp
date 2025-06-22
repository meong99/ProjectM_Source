#include "MDefaultShopWidget.h"
#include "Components/Button.h"
#include "MDefaultShopDetailWidget.h"
#include "Interaction/MInteractiveAction_Base.h"
#include "Components/TextBlock.h"
#include "Interaction/MInteractiveAction_OpenShop.h"
#include "Character/Components/MWalletComponent.h"

UMDefaultShopWidget::UMDefaultShopWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMDefaultShopWidget::PreAddToLayer()
{
	ExitButton->OnClicked.AddDynamic(this, &UMDefaultShopWidget::OnClicked_ExitButton);
}

void UMDefaultShopWidget::NativeConstruct()
{
	ShopDetail->SetWidgetInfo(WidgetInfo);
	UserInventoryDetail->SetWidgetInfo(WidgetInfo);

	UMInteractiveAction_OpenShop* ShopAction = Cast<UMInteractiveAction_OpenShop>(WidgetInfo.WidgetInstigator);
	if (ShopAction)
	{
		const FMShopDefinition& ShopDefinition = ShopAction->GetShopDefinition();
		ShopName->SetText(ShopDefinition.ShopName);
	}

	APlayerController* PlayerController = GetOwningPlayer();
	UMWalletComponent* PlayerWalletComp = PlayerController ? PlayerController->FindComponentByClass<UMWalletComponent>() : nullptr;
	if (PlayerWalletComp)
	{
		PlayerWalletComp->Delegate_OnChangeGold.AddDynamic(this, &UMDefaultShopWidget::OnChange_Gold);
		OwnedGold->SetText(FText::AsNumber(PlayerWalletComp->GetGold()));
	}
}

void UMDefaultShopWidget::NativeDestruct()
{
	APlayerController* PlayerController = GetOwningPlayer();
	UMWalletComponent* PlayerWalletComp = PlayerController ? PlayerController->FindComponentByClass<UMWalletComponent>() : nullptr;
	if (PlayerWalletComp && PlayerWalletComp->Delegate_OnChangeGold.IsAlreadyBound(this, &UMDefaultShopWidget::OnChange_Gold))
	{
		PlayerWalletComp->Delegate_OnChangeGold.RemoveDynamic(this, &UMDefaultShopWidget::OnChange_Gold);
	}

	DeactivateShop();

	Super::NativeDestruct();
}

void UMDefaultShopWidget::DeactivateShop()
{
	UMInteractiveAction_Base* ActionBase = Cast<UMInteractiveAction_Base>(WidgetInfo.WidgetInstigator);
	if (ActionBase)
	{
		ActionBase->DeactivateAction();
	}
}

void UMDefaultShopWidget::OnClicked_ExitButton()
{
	RemoveWidgetFromLayer();
}

void UMDefaultShopWidget::OnChange_Gold(int64 AdjustNum, int64 NewGold)
{
	OwnedGold->SetText(FText::AsNumber(NewGold));
}
