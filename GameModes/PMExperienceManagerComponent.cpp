#include "PMExperienceManagerComponent.h"
#include "DataAssets/PMAssetManager.h"
#include "PMExperienceDefinition.h"
#include "GameFeaturesSubsystemSettings.h"
#include "GameFeaturesSubsystem.h"
#include "GameFeatureAction.h"
#include "PMExperienceActionSet.h"
#include "Net/UnrealNetwork.h"

UPMExperienceManagerComponent::UPMExperienceManagerComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UPMExperienceManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	for (const FString& PluginURL : GameFeaturePluginURLs)
	{
		UGameFeaturesSubsystem::Get().DeactivateGameFeaturePlugin(PluginURL);
	}

	if (LoadState == EPMExperienceLoadState::Loaded)
	{
		LoadState = EPMExperienceLoadState::Deactivating;

		NumExpectedPausers = INDEX_NONE;
		NumObservedPausers = 0;
		FGameFeatureDeactivatingContext Context(TEXT(""), [this](FStringView) { this->OnActionDeactivationCompleted(); });

		const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
		if (ExistingWorldContext)
		{
			Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
		}

		auto DeactivateListOfActions = [&Context](const TArray<UGameFeatureAction*>& ActionList)
		{
			for (UGameFeatureAction* Action : ActionList)
			{
				if (Action)
				{
					MCHAE_LOG("Action Deactivate. ActionName = %s", *Action->GetName());
					Action->OnGameFeatureDeactivating(Context);
					Action->OnGameFeatureUnregistering();
				}
			}
		};

		DeactivateListOfActions(CurrentExperience->Actions);
		for (const TObjectPtr<UPMExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
		{
			if (ActionSet != nullptr)
			{
				DeactivateListOfActions(ActionSet->Actions);
			}
		}

		NumExpectedPausers = Context.GetNumPausers();

		if (NumExpectedPausers > 0)
		{
			MCHAE_ERROR("Actions that have asynchronous deactivation aren't fully supported yet in Lyra experiences");
		}

		if (NumExpectedPausers == NumObservedPausers)
		{
			OnAllActionsDeactivated();
		}
	}
}

void UPMExperienceManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentExperience);
}

void UPMExperienceManagerComponent::SetCurrentExperience(FPrimaryAssetId ExperienceId)
{
	UPMAssetManager& AssetManager = UPMAssetManager::Get();

	TSubclassOf<UPMExperienceDefinition> AssetClass;
	FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(ExperienceId);
	AssetClass = Cast<UClass>(AssetPath.TryLoad());

	const UPMExperienceDefinition* Experience = GetDefault<UPMExperienceDefinition>(AssetClass);
	check(Experience != nullptr);

	check(CurrentExperience == nullptr)
	CurrentExperience = Experience;

	StartExperienceLoad();
}

void UPMExperienceManagerComponent::CallOrRegister_OnExperienceLoaded(FOnExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded())
	{
		Delegate.Execute(CurrentExperience);
	}
	else
	{
		OnExperienceLoaded.Add(MoveTemp(Delegate));
	}
}

void UPMExperienceManagerComponent::StartExperienceLoad()
{
	check(CurrentExperience);
	check(LoadState == EPMExperienceLoadState::Unloaded);

	LoadState = EPMExperienceLoadState::Loading;

	UPMAssetManager& AssetManager = UPMAssetManager::Get();

	/*
	* 이미 ExperienceId를 넘겨줬는데 왜 CDO를 사용하는걸까?
	* GetPrimaryAssetID의 고유 특성때문에 그렇다. CDO를 사용하지 않으면 제대로 로딩되지 않는다. 내부코드를 보면 알 수 있다.
	*/
	TSet<FPrimaryAssetId> BundleAssetList;
	BundleAssetList.Add(CurrentExperience->GetPrimaryAssetId());

	// Widget같이 특정 상황에서 Loading을 해줘야 한다면 Soft로 묶여있는데 이건 별도로 진행해줘야한다.
	for (const TObjectPtr<UPMExperienceActionSet>& ActionSet : CurrentExperience->GetActionSets())
	{
		if (ActionSet)
		{
			/*
			* HUD 추가하는 과정이다.
			* 자세한 과정은 UAssetManager::ChageBundleStateForPrimaryAssets를 보자
			*/
			BundleAssetList.Add(ActionSet->GetPrimaryAssetId());
		}
	}

	TArray<FName> BundlesToLoad;
	const ENetMode OwnerNetMode = GetOwner()->GetNetMode();
	bool bLoadClient = GIsEditor || OwnerNetMode != NM_DedicatedServer;
	bool bLoadServer = GIsEditor || OwnerNetMode != NM_Client;
	if (bLoadClient)
	{
		BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateClient);
	}
	if (bLoadServer)
	{
		BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateServer);
	}

	FStreamableDelegate OnAssetsLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ThisClass::OnExperienceLoadComplete);
	TSharedPtr<FStreamableHandle> Handle = AssetManager.ChangeBundleStateForPrimaryAssets(
		BundleAssetList.Array(),
		BundlesToLoad,
		{},
		false,
		FStreamableDelegate(),
		FStreamableManager::AsyncLoadHighPriority
	);

	if (Handle.IsValid() == false || Handle->HasLoadCompleted())
	{
		FStreamableHandle::ExecuteDelegate(OnAssetsLoadedDelegate);
	}
	else
	{
		Handle->BindCompleteDelegate(OnAssetsLoadedDelegate);
		Handle->BindCancelDelegate(FStreamableDelegate::CreateLambda([OnAssetsLoadedDelegate]() -> void
		{
			OnAssetsLoadedDelegate.ExecuteIfBound();	
		}));
	}
}

void UPMExperienceManagerComponent::OnExperienceLoadComplete()
{
	static int32 OnExperienceLoadComplete_FrameNumber = GFrameNumber;

	check(LoadState == EPMExperienceLoadState::Loading);
	check(CurrentExperience);

	GameFeaturePluginURLs.Reset();

	auto CollectGameFeaturePluginURLs = [This = this](const UPrimaryDataAsset* Context, const TArray<FString>& FeaturePluginList) -> void
		{
			for (const FString& PluginName : FeaturePluginList)
			{
				// ExperienceDefinition에 정의되어있는 활성화시키고싶은 Experience의 URL(파일단위)을 가져와주고, 저장한다.
				FString PluginURL;
				if (UGameFeaturesSubsystem::Get().GetPluginURLByName(PluginName, PluginURL))
				{
					This->GameFeaturePluginURLs.AddUnique(PluginURL);
				}
			}
		};

	CollectGameFeaturePluginURLs(CurrentExperience, CurrentExperience->GameFeaturesToEnable);

	NumGameFeaturePluginsLoading = GameFeaturePluginURLs.Num();
	if (NumGameFeaturePluginsLoading)
	{
		LoadState = EPMExperienceLoadState::LoadingGameFeatures;
		for (const FString& PluginURL : GameFeaturePluginURLs)
		{
			UGameFeaturesSubsystem::Get().LoadAndActivateGameFeaturePlugin(PluginURL, FGameFeaturePluginLoadComplete::CreateUObject(this, &ThisClass::OnGameFeaturePluginLoadComplete));
		}
	}
	else
	{
		OnExperienceFullLoadCompleted();
	}
}

void UPMExperienceManagerComponent::OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result)
{
	--NumGameFeaturePluginsLoading;
	if (NumGameFeaturePluginsLoading == 0)
	{
		OnExperienceFullLoadCompleted();
	}
}

void UPMExperienceManagerComponent::OnExperienceFullLoadCompleted()
{
	check(LoadState != EPMExperienceLoadState::Loaded);

	// 활성화 시키고싶은 GameFeature를 모두 활성화 시켰으니 이제 Action들을 적용시킨다
	LoadState = EPMExperienceLoadState::ExecutingActions;

	// GameFeatureSubsystem은 Engine Subsystem이다. 그 말은 즉, 게임이 시작되고 어떤 레벨을 이동하던지, PIE를 얼마나 켜고 끄던지 엔진이 살아있는동안 계속 살아있는다는 뜻이다.
	// 그렇다는말은 GameFeature를 관리할 때 어떤 단위로 관리해야하는지 지정해줘야한다는 말인데 그걸 WorldContext로 지정해주는 것이다.
	FGameFeatureActivatingContext Context;
	const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
	if (ExistingWorldContext)
	{
		Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
	}

	auto ActivateListOfActions = [&Context](const TArray<UGameFeatureAction*>& ActionList) -> void
		{
			for (UGameFeatureAction* Action : ActionList)
			{
				if (Action)
				{
					Action->OnGameFeatureRegistering();
					Action->OnGameFeatureLoading();
					Action->OnGameFeatureActivating(Context);
				}
			}
		};

	ActivateListOfActions(CurrentExperience->GetActions());

	for (const TObjectPtr<UPMExperienceActionSet>& ActionSet : CurrentExperience->GetActionSets())
	{
		ActivateListOfActions(ActionSet->Actions);
	}

	LoadState = EPMExperienceLoadState::Loaded;
	OnExperienceLoaded.Broadcast(CurrentExperience);
	OnExperienceLoaded.Clear();
}

void UPMExperienceManagerComponent::OnRep_CurrentExperience()
{
	StartExperienceLoad();
}

void UPMExperienceManagerComponent::OnActionDeactivationCompleted()
{
	check(IsInGameThread());
	++NumObservedPausers;

	if (NumObservedPausers == NumExpectedPausers)
	{
		OnAllActionsDeactivated();
	}
}

void UPMExperienceManagerComponent::OnAllActionsDeactivated()
{
	LoadState = EPMExperienceLoadState::Unloaded;
	CurrentExperience = nullptr;
}

const UPMExperienceDefinition* UPMExperienceManagerComponent::GetCurrentExperienceChecked() const
{
	check(LoadState == EPMExperienceLoadState::Loaded);
	check(CurrentExperience != nullptr);

	return CurrentExperience;
}
