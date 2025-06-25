#include "GameFeatureAction_WorldActionBase.h"
#include "GameFeaturesSubsystem.h"

UGameFeatureAction_WorldActionBase::UGameFeatureAction_WorldActionBase()
{
}

void UGameFeatureAction_WorldActionBase::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		if (Context.ShouldApplyToWorldContext(WorldContext))
		{
			AddToWorld(WorldContext, Context);
		}
	}
}
