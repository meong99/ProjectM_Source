#pragma once

#include "Engine/DataAsset.h"
#include "PMUserFacingExperienceDefinition.generated.h"

class UCommonSession_HostSessionRequest;

UCLASS()
class PROJECTM_API UPMUserFacingExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	UPMUserFacingExperienceDefinition();

/*
* Member Functions
*/
public:
	/*
	* @Warning 내부에서 NewObject를 이용해 생성하므로 생성한 Frame 내에 해당 오브젝트를 사용해야한다. 그렇지 않으면 Dangling이 날 수 있다.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	UCommonSession_HostSessionRequest* CreateHostingRequst(const FString& Ip, bool bIsSinglePlay) const;
	
	FString GetMapName() const;
/*
* Member Variables
*/
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true, AllowedTypes = "Map"), Category = "ProjectM | Experience")
	FPrimaryAssetId MapId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true, AllowedTypes = "PMExperienceDefinition"), Category = "ProjectM | Experience")
	FPrimaryAssetId ExperienceId;
};