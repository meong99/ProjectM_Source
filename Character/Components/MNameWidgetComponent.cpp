#include "Character/Components/MNameWidgetComponent.h"
#include "Character/NPC/MNpcBase.h"
#include "UI/Name/MNameWidget.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Character/MCharacterBase.h"

UMNameWidgetComponent::UMNameWidgetComponent()
{
	PrimaryComponentTick.bAllowTickOnDedicatedServer = false;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	TickMode = ETickMode::Disabled;
	Space = EWidgetSpace::Screen;
}

void UMNameWidgetComponent::InitWidget()
{
	Super::InitWidget();

	NameWidget = Cast<UMNameWidget>(GetWidget());
	WeakOwnerCharacter = Cast<ACharacter>(GetOwner());
}

void UMNameWidgetComponent::EnableNameWidget()
{
	if (NameWidget)
	{
		NameWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SetTickMode(ETickMode::Enabled);
	}
}

void UMNameWidgetComponent::DisableNameWidget()
{
	if (NameWidget)
	{
		SetTickMode(ETickMode::Disabled);
		NameWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMNameWidgetComponent::InitNameWidget()
{
	AMCharacterBase* CharacterBase = Cast<AMCharacterBase>(WeakOwnerCharacter.Get());
	if (NameWidget && CharacterBase)
	{
		FName Name = CharacterBase->GetCharacterName();

		NameWidget->SetName(Name);
		NameWidget->SetWidgetInfo({this, GetOwner()});
	}
}

void UMNameWidgetComponent::AdjustNameWidgetCompLocation()
{
	UCapsuleComponent* Capsule = WeakOwnerCharacter->GetCapsuleComponent();
	if (Capsule)
	{
		float Height = Capsule->GetScaledCapsuleHalfHeight();
		SetRelativeLocation(FVector::UpVector * (Height * 1.5));
	}
}
