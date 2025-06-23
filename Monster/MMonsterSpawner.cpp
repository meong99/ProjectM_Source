#include "MMonsterSpawner.h"
#include "Character/Monster/MMonsterTypes.h"
#include "Engine/Engine.h"
#include "Util/MGameplayStatics.h"
#include "System/MDataTableManager.h"
#include "Definitions/MMonsterDefinition.h"
#include "Engine/World.h"
#include "MMonsterBase.h"
#include "Components/SplineComponent.h"
#include "GameModes/PMGameStateBase.h"
#include "Components/BillboardComponent.h"

AMMonsterSpawner::AMMonsterSpawner(const FObjectInitializer& ObjectInitializer)
{
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	RootComponent = SplineComponent;
	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("BillboardComponent"));
	BillboardComponent->SetupAttachment(RootComponent);
	BillboardComponent->SetRelativeLocation({ 0.0f, 0.0f , 100.0f });
	PrimaryActorTick.bCanEverTick = true;
}

void AMMonsterSpawner::BeginPlay()
{
	Super::BeginPlay();

	MonsterDefinition = UMDataTableManager::GetDefinitionObject<UMMonsterDefinition>(this, MonsterRowId);

	if (HasAuthority() && MonsterDefinition)
	{
		for (int32 i = 0; i < MonsterDefinition->GetMaximumSpawnNum() - SpawnedMonsters.Num(); i++)
		{
			SpawnMonster();
		}
	}
	else
	{
		SetActorTickEnabled(false);
	}

	APMGameStateBase* GameState = Cast<APMGameStateBase>(GetWorld()->GetGameState());
	if (GameState && SearchTag.IsValid())
	{
		GameState->TagMappedActor.Add(SearchTag, this);
	}
}

void AMMonsterSpawner::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HasAuthority())
	{
		if (MonsterDefinition && SpawnedMonsters.Num() < MonsterDefinition->GetMaximumSpawnNum())
		{
			SpawnTimer += DeltaSeconds;
			if (SpawnTimer >= MonsterDefinition->GetMonsterSpawningCooldown())
			{
				SpawnMonster();
			}
		}
		else
		{
			SpawnTimer = 0.0f;
		}
	}
}

void AMMonsterSpawner::OnDeadMonster(AMMonsterBase* DeadMonster)
{
	if (DeadMonster)
	{
		SpawnedMonsters.Remove(DeadMonster);
	}
}

void AMMonsterSpawner::SpawnMonster()
{
	if (MonsterDefinition->GetMaximumSpawnNum() <= SpawnedMonsters.Num() || !HasAuthority())
	{
		return;
	}

	AMMonsterBase* SpawnedMonster = GetWorld()->SpawnActorDeferred<AMMonsterBase>(MonsterDefinition->GetMonsterClass(), FTransform::Identity);
	if (IsValid(SpawnedMonster))
	{
		FTransform			Transform;
		int32				PointIndex = FMath::RandHelper(SplineComponent->GetNumberOfSplinePoints());
		const FSplinePoint& Point = SplineComponent->GetSplinePointAt(PointIndex, ESplineCoordinateSpace::Local);
		FHitResult			FloorHit = UMGameplayStatics::GetFloorLocation(this, Point.Position + GetActorLocation(), { this }, 1500.f);
		FVector				Location = FloorHit.bBlockingHit ? FloorHit.Location : Point.Position;

		Transform.SetLocation(Location);

		SpawnedMonster->SetSpawner(this);
		SpawnedMonster->FinishSpawning(Transform);
		SpawnedMonster->SpawnDefaultController();
		SpawnedMonsters.Add(SpawnedMonster);
	}
}
