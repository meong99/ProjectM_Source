#include "MDialogueBoxWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Util/MGameplayStatics.h"
#include "System/MDataTableManager.h"
#include "Table/MTable_DialogueTable.h"
#include "Character/MCharacterBase.h"
#include "Interaction/MInteractiveAction_AcceptQuest.h"

UMDialogueBoxWidget::UMDialogueBoxWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMDialogueBoxWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (DialogTextArray.IsEmpty())
	{
		ensure(false);
		RemoveWidgetFromLayer();
		return;
	}

	StartDialogue();
}

void UMDialogueBoxWidget::NativeDestruct()
{
	ClearDialogue();
	Super::NativeDestruct();
}

FReply UMDialogueBoxWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	TurnOverPage();

	return FReply::Handled();
}

void UMDialogueBoxWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	CallbackButton1->OnClicked.AddDynamic(this, &UMDialogueBoxWidget::OnClick_Button1);
	CallbackButton2->OnClicked.AddDynamic(this, &UMDialogueBoxWidget::OnClick_Button2);
	ExitButton->OnClicked.AddDynamic(this, &UMDialogueBoxWidget::OnClick_ExitButton);
}

void UMDialogueBoxWidget::InitDialogueWidget(const int32 DialogueRowId)
{
	UMDataTableManager* TableManager = UMGameplayStatics::GetDataTableManager();
	if (TableManager)
	{
		const UDataTable* DialogueTable = TableManager->GetDataTable(DialogueRowId);
		if (DialogueTable)
		{
			int32 ElementIndex = UMDataTableManager::ChangeRowIdToElementId(DialogueRowId) - 1;
			const TArray<FName>& Names = DialogueTable->GetRowNames();
			if (Names.IsValidIndex(ElementIndex))
			{
				FMTable_DialogueTable* RowData = DialogueTable->FindRow<FMTable_DialogueTable>(Names[ElementIndex], Names[ElementIndex].ToString());
				if (RowData)
				{
					DialogTextArray = RowData->DialogueArray;

					Button1Text->SetText(RowData->Callback1_TextName);
					Button2Text->SetText(RowData->Callback2_TextName);
				}
			}
		}
	}

	AMCharacterBase* OwnerCharacter = Cast<AMCharacterBase>(WidgetInfo.WidgetOwnerActor);
	if (OwnerCharacter)
	{
		SetOwnerName(FText::FromString(OwnerCharacter->GetCharacterName().ToString()));
	}
}

void UMDialogueBoxWidget::SetCallback1(TFunction<void()> Callback)
{
	if (Callback)
	{
		Callback1 = Callback;
		CallbackButton1->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		CallbackButton1->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMDialogueBoxWidget::SetCallback2(TFunction<void()> Callback)
{
	if (Callback)
	{
		Callback2 = Callback;
		CallbackButton2->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		CallbackButton2->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMDialogueBoxWidget::StartDialogue()
{
	SetDialogue();
}

void UMDialogueBoxWidget::SetDialogue()
{
	if (DialogTextArray.IsValidIndex(CurrentPage))
	{
		FText Dialog = DialogTextArray[CurrentPage];
		MainDialog->SetText(Dialog);
	}
}

void UMDialogueBoxWidget::TurnOverPage()
{
	CurrentPage++;
	if (DialogTextArray.IsValidIndex(CurrentPage))
	{
		if (DialogTextArray.Num() - 1 == CurrentPage)
		{
			ButtonBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		SetDialogue();
	}
}

void UMDialogueBoxWidget::ClearDialogue()
{
	CurrentPage = 0;
	DialogTextArray.Empty();
	ButtonBox->SetVisibility(ESlateVisibility::Collapsed);
	UMInteractiveAction_AcceptQuest* Action = Cast<UMInteractiveAction_AcceptQuest>(WidgetInfo.WidgetInstigator);
	Callback1.Reset();
	Callback2.Reset();
	if (Action)
	{
		Action->DeactivateAction();
	}
}

void UMDialogueBoxWidget::OnClick_Button1()
{
	if (Callback1)
	{
		Callback1();
		RemoveWidgetFromLayer();
	}
}

void UMDialogueBoxWidget::OnClick_Button2()
{
	if (Callback2)
	{
		Callback2();
		RemoveWidgetFromLayer();
	}
}

void UMDialogueBoxWidget::OnClick_ExitButton()
{
	RemoveWidgetFromLayer();
}

void UMDialogueBoxWidget::SetOwnerName(const FText InOwnerName)
{
	OwnerName->SetText(InOwnerName);
}

void UMDialogueBoxWidget::SetButton1_Text(const FText& InText)
{
	Button1Text->SetText(InText);
}

void UMDialogueBoxWidget::SetButton2_Text(const FText& InText)
{
	Button2Text->SetText(InText);
}
