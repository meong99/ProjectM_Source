#include "MLevelTagTriggerBox.h"
#include "Components/ShapeComponent.h"
#include "Character/MPlayerCharacterBase.h"
#include "Util/MGameplayStatics.h"

AMLevelTagTriggerBox::AMLevelTagTriggerBox()
{
	if (GetCollisionComponent())
	{
		GetCollisionComponent()->SetCollisionProfileName(*UEnum::GetDisplayValueAsText(EMCollisionChannel::Interaction).ToString());
	}
}

void AMLevelTagTriggerBox::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &AMLevelTagTriggerBox::OnBeginOverlap);
}

void AMLevelTagTriggerBox::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMPlayerCharacterBase* Player = Cast<AMPlayerCharacterBase>(OtherActor);
	if (Player)
	{
		Player->SetCurrentLevelTag(GrantedLevelTag);
	}
}
