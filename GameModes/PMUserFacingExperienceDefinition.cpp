#include "PMUserFacingExperienceDefinition.h"
#include "CommonSessionSubSystem.h"
#include "Engine/AssetManager.h"
#include "AssetRegistry/AssetData.h"

UPMUserFacingExperienceDefinition::UPMUserFacingExperienceDefinition()
{

}

UCommonSession_HostSessionRequest* UPMUserFacingExperienceDefinition::CreateHostingRequst(const FString& Ip, bool bIsSinglePlay) const
{
    const FString ExperienceName = ExperienceId.PrimaryAssetName.ToString();

	MCHAE_LOG("ExperienceName = %s", *ExperienceName);

    // NewObject를 생성해서 바로 사용하지 않으면 GC로 인해 Dangling이 날 수 있다.
    UCommonSession_HostSessionRequest* Result = NewObject<UCommonSession_HostSessionRequest>();
	if (!Ip.IsEmpty() && !bIsSinglePlay)
	{
		Result->URL = Ip;
	}
	else
	{
		Result->URL= GetMapName();
		if (Result->URL.IsEmpty())
		{
			Result->URL = MapId.PrimaryAssetName.ToString();
		}
	}

	MCHAE_LOG("MapName = %s", *MapId.PrimaryAssetName.ToString());
	MCHAE_LOG("URL = %s", *Result->URL);
	MCHAE_LOG("IP = %s", *Ip);
	Result->ExtraArgs.Add(TEXT("Experience"), ExperienceName);

    return Result;
}

FString UPMUserFacingExperienceDefinition::GetMapName() const
{
	FAssetData MapAssetData;
	if (UAssetManager::Get().GetPrimaryAssetData(MapId, MapAssetData))
	{
		MCHAE_LOG("GetAssetData");
		return MapAssetData.PackageName.ToString();
	}

	MCHAE_LOG("Can't GetAssetData");
	return FString();
}
