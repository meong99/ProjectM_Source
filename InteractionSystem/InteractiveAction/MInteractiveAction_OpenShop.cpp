#include "MInteractiveAction_OpenShop.h"
#include "MInteractionComponent.h"
#include "UI/MViewportClient.h"
#include "PMGameplayTags.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "System/MDataTableManager.h"
#include "Engine/Engine.h"
#include "Table/Item/MTable_Shop.h"

UMInteractiveAction_OpenShop::UMInteractiveAction_OpenShop(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMInteractiveAction_OpenShop::InitAction(UMInteractionComponent* InInteractionComponent, AActor* InOwnerActor)
{
	Super::InitAction(InInteractionComponent, InOwnerActor);

	UMDataTableManager* TableManager = GEngine->GetEngineSubsystem<UMDataTableManager>();
	if (TableManager)
	{
		const UDataTable* ShopTable = TableManager->GetDataTable(ShopRowId);
		if (ShopTable)
		{
			int32 ElementIndex = UMDataTableManager::ChangeRowIdToElementId(ShopRowId) - 1;
			const TArray<FName>& Names = ShopTable->GetRowNames();
			if (Names.IsValidIndex(ElementIndex))
			{
				FMTable_Shop* RowData = ShopTable->FindRow<FMTable_Shop>(Names[ElementIndex], Names[ElementIndex].ToString());
				if (RowData)
				{
					ShopDefinition = RowData->ShopDefinition;
				}
			}
		}
	}
}

void UMInteractiveAction_OpenShop::ActivateAction()
{
	Super::ActivateAction();

	UMViewportClient* ViewportClient = UMViewportClient::Get(this);
	if (ViewportClient)
	{
		bIsActivated = true;
		
		FMWidgetInfo WidgetInfo;
		WidgetInfo.WidgetInstigator = this;
		WidgetInfo.WidgetOwnerActor = OwnerActor;

		ViewportClient->AddWidgetToLayer(FPMGameplayTags::Get().UI_Registry_Game_Shop, WidgetInfo);
	}
}

void UMInteractiveAction_OpenShop::DeactivateAction()
{
	bIsActivated = false;
	UMViewportClient* ViewportClient = Cast<UMViewportClient>(GetWorld()->GetGameInstance()->GetGameViewportClient());
	if (ViewportClient && InteractionComponent)
	{
		ViewportClient->RemoveWidgetFromLayer(FPMGameplayTags::Get().UI_Registry_Game_Shop);
	}
	Super::DeactivateAction();
}
