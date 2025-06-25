#include "PMGameModeBase.h"
#include "Character/MPlayerCharacterBase.h"
#include "PMGameStateBase.h"
#include "Player/PMPlayerControllerBase.h"
#include "Player/PMPlayerState.h"
#include "PMExperienceDefinition.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "PMExperienceManagerComponent.h"
#include "Character/PMPawnData.h"
#include "TimerManager.h"
#include "Character/Components/PMPawnExtensionComponent.h"
#include "UI/PMHUD.h"
#include "Misc/CommandLine.h"
#include "MWorldSettings.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "PMGameplayTags.h"

APMGameModeBase::APMGameModeBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	GameStateClass = APMGameStateBase::StaticClass();
	DefaultPawnClass = AMPlayerCharacterBase::StaticClass();
	PlayerControllerClass = APMPlayerControllerBase::StaticClass();
	PlayerStateClass = APMPlayerState::StaticClass();
	HUDClass = APMHUD::StaticClass();
}

void APMGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}

void APMGameModeBase::InitGameState()
{
	Super::InitGameState();

	UPMExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UPMExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

UClass* APMGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const UPMPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->GetPawnClass())
		{
			return PawnData->GetPawnClass();
		}
	}
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void APMGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	if (IsExperienceLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}

APawn* APMGameModeBase::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;
	SpawnInfo.bDeferConstruction = true;

	UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer);
	if (IsValid(PawnClass) == false)
	{
		MCHAE_LOG("Pawn Class's invalid");
		return nullptr;
	}

	APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo);
	if (IsValid(SpawnedPawn) == false)
	{
		MCHAE_LOG("Pawn was not spawned");
		return nullptr;
	}

	UPMPawnExtensionComponent* PawnExtComp = UPMPawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn);
	if (IsValid(PawnExtComp) == false)
	{
		MCHAE_LOG("PawnExtComp's invalid");
		return nullptr;
	}

	if (const UPMPawnData* PawnData = GetPawnDataForController(NewPlayer))
	{
		PawnExtComp->SetPawnData(PawnData);
	}

	SpawnedPawn->FinishSpawning(SpawnTransform);

	return SpawnedPawn;
}

AActor* APMGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	UWorld* World = GetWorld();
	AMWorldSettings* WorldSetting = Cast<AMWorldSettings>(World->GetWorldSettings(true));
	if (WorldSetting && WorldSetting->WorldTag.MatchesAny(FGameplayTagContainer( FPMGameplayTags::Get().Level_Persistent) ))
	{
		APlayerStart* FoundPlayerStart = nullptr;
		UClass* PawnClass = GetDefaultPawnClassForController(Player);
		APawn* PawnToFit = PawnClass ? PawnClass->GetDefaultObject<APawn>() : nullptr;
		TArray<APlayerStart*> UnOccupiedStartPoints;
		TArray<APlayerStart*> OccupiedStartPoints;

		for (TActorIterator<APlayerStart> It(World); It; ++It)
		{
			APlayerStart* PlayerStart = *It;

			if (FoundPlayerStart == nullptr || PlayerStart->PlayerStartTag == TEXT("Square"))
			{
				FoundPlayerStart = PlayerStart;
			}
		}
		return FoundPlayerStart;
	}

	return Super::ChoosePlayerStart_Implementation(Player);
}

bool APMGameModeBase::IsExperienceLoaded() const
{
	check(GameState);

	const UPMExperienceManagerComponent* ExperienceManagerComp = GameState->FindComponentByClass<UPMExperienceManagerComponent>();
	check(ExperienceManagerComp);

	return ExperienceManagerComp->IsExperienceLoaded();
}

const UPMPawnData* APMGameModeBase::GetPawnDataForController(const AController* InController) const
{
	// PlayerState에 캐싱되어있는 PawnData를 우선적으로 사용하도록 한다.
	if (IsValid(InController))
	{
		if (const APMPlayerState* PlayerState = InController->GetPlayerState<APMPlayerState>())
		{
			if (const UPMPawnData* PawnData = PlayerState->GetPawnData<UPMPawnData>())
			{
				return PawnData;
			}
		}
	}

	check(GameState);

	// PlayerState에 아직 캐싱되어있지 않은 상태라면 ExperienceManager에서 현재 Experience를 요청해 설정한다.
	const UPMExperienceManagerComponent* ExperienceManagerComp = GameState->FindComponentByClass<UPMExperienceManagerComponent>();
	check(ExperienceManagerComp);

	if (ExperienceManagerComp->IsExperienceLoaded())
	{
		const UPMExperienceDefinition* Experience = ExperienceManagerComp->GetCurrentExperienceChecked();
		if (Experience && Experience->GetDefaultPawnData())
		{
			return Experience->GetDefaultPawnData();
		}
	}

	return nullptr;
}

void APMGameModeBase::HandleMatchAssignmentIfNotExpectingOne()
{
	FPrimaryAssetId ExperienceId;
	UWorld* World = GetWorld();

	// Option으로 넘겨준 값을 이용해 ExperienceDefinition을 찾는다. 이 Option값은 레벨 이동하기 전 UserfacingExperience에서 세팅해주고 Travel을 한다.
	// 그렇게 되면 GameMode의 OptionString이 채워지고, 우리는 그 OptionString을 이용해서 원하는 ExperienceDefinition을 가져오는 것이다.
	if (ExperienceId.IsValid() == false && UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
	{
		const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType(UPMExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromOptions/*얘가 ExperienceDefinition 이름*/));
	}

	// CommandLine으로 Experience읽기
	if (!ExperienceId.IsValid())
	{
		FString ExperienceFromCommandLine;
		if (FParse::Value(FCommandLine::Get(), TEXT("Experience="), ExperienceFromCommandLine))
		{
			ExperienceId = FPrimaryAssetId::ParseTypeAndName(ExperienceFromCommandLine);
			if (!ExperienceId.PrimaryAssetType.IsValid())
			{
				ExperienceId = FPrimaryAssetId(FPrimaryAssetType(UPMExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromCommandLine));
			}
		}
	}

	if (!ExperienceId.IsValid())
	{
		if (AMWorldSettings* TypedWorldSettings = Cast<AMWorldSettings>(GetWorldSettings()))
		{
			ExperienceId = TypedWorldSettings->GetDefaultGameplayExperience();
		}
	}

	// Experience를 마지막까지 못 찾는다면 기본값을 사용한다.
	if (ExperienceId.IsValid() == false)
	{
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType(FPrimaryAssetType("PMExperienceDefinition")), FName("BP_DefautExperienceDefinition"));
	}

	OnMatchAssignmentGiven(ExperienceId);
}

void APMGameModeBase::OnMatchAssignmentGiven(const FPrimaryAssetId& ExperienceId)
{
	if (ExperienceId.IsValid() && GameState)
	{
		UPMExperienceManagerComponent* ExperienceManagerComp = GameState->FindComponentByClass<UPMExperienceManagerComponent>();
		check(ExperienceManagerComp);

		ExperienceManagerComp->SetCurrentExperience(ExperienceId);
	}
	else
	{
		MCHAE_ERROR("Failed to identify experience, loading screen will stay up forever");
	}
}

void APMGameModeBase::OnExperienceLoaded(const UPMExperienceDefinition* CurrentExperience)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Cast<APlayerController>(*Iterator);
		if (PlayerController && PlayerController->GetPawn() == nullptr)
		{
			if (PlayerCanRestart(PlayerController))
			{
				RestartPlayer(PlayerController);
			}
		}
	}
}

bool APMGameModeBase::TryDedicatedServerLogin()
{
	return false;
}
