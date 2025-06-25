#include "AsyncAction_ExperienceReady.h"
#include "Engine/World.h"
#include "PMExperienceManagerComponent.h"
#include "TimerManager.h"
#include "Engine/Engine.h"

UAsyncAction_ExperienceReady* UAsyncAction_ExperienceReady::WaitForExperienceReady(UObject* WorldContextObject)
{
	UAsyncAction_ExperienceReady* Action = nullptr;
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		Action = NewObject<UAsyncAction_ExperienceReady>();
		Action->WorldPtr = World;
		Action->RegisterWithGameInstance(World);
	}

	return Action;
}

UAsyncAction_ExperienceReady::UAsyncAction_ExperienceReady()
{

}

void UAsyncAction_ExperienceReady::Activate()
{
	if (UWorld* World = WorldPtr.Get())
	{
		if (AGameStateBase* GameState = World->GetGameState())
		{
			Step2_ListenToExperienceLoading(GameState);
		}
		else
		{
			World->GameStateSetEvent.AddUObject(this, &ThisClass::Step1_HandleGameStateSet);
		}
	}
	else
	{
	// BlueprintAsyncAction같은 경우에는 GameInstance에 캐싱되어있다. 그래서 캐싱된 데이터를 지우면 GC에 의해서 완전히 없어진다.
		SetReadyToDestroy();
	}
}

void UAsyncAction_ExperienceReady::Step1_HandleGameStateSet(AGameStateBase* GameState)
{
	if (UWorld* World = WorldPtr.Get())
	{
		World->GameStateSetEvent.RemoveAll(this);
	}

	Step2_ListenToExperienceLoading(GameState);
}

void UAsyncAction_ExperienceReady::Step2_ListenToExperienceLoading(AGameStateBase* GameState)
{
	check(GameState);

	UPMExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UPMExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	if (ExperienceManagerComponent->IsExperienceLoaded())
	{
		UWorld* World = GameState->GetWorld();
		check(World);

		// Mesh장착은 Controller에서 OnPossess에 Binding된 Delegate에서 호출되는데 Experience가 Load되는 타이밍과는 정확하게 맞지 않을 수 있다.
		// 그래서 Mesh가 완전히 장착된 후에 아이템이 장착되도록 하기 위해서 다음 프레임에 동작하도록 한다.
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ThisClass::Step4_BroadcastReady));
	}
	else
	{
		ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::Step3_HandleExperienceLoaded));
	}
}

void UAsyncAction_ExperienceReady::Step3_HandleExperienceLoaded(const UPMExperienceDefinition* CurrentExperience)
{
	Step4_BroadcastReady();
}

void UAsyncAction_ExperienceReady::Step4_BroadcastReady()
{
	OnReady.Broadcast();
	SetReadyToDestroy();
}