#pragma once

#include "CoreMinimal.h"
#include "MDefinitionBase.h"
#include "Types/MQuestTypes.h"
#include "Types/MTradeType.h"
#include "GameplayTagContainer.h"
#include "MQuestDefinition.generated.h"

class AMNpcBase;

UCLASS(Blueprintable, BlueprintType)
class PROJECTM_API UMQuestDefinition : public UMDefinitionBase
{
	GENERATED_BODY()

/*
* Overrided Functions
*/
public:
	UMQuestDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

/*
* Member Functions
*/
public:
	FMTradeRequest CreateTradeRequest() const;

/*
* Member Variables
*/
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (), Category = "Quest Infomation")
	int32 OwnerNpcRowId;

	// 레벨 내에 특정 목적지가 있을 때 사용하는 태그
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (), Category = "Quest Infomation")
	FGameplayTag SubSearchTag;

	// 기본적으로 시작 가능한 퀘스트로 지정되는지
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(), Category="Quest Infomation")
	bool bAddToDefault = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(), Category="Quest Infomation")
	FText QuestName;

	// 퀘스트 목표
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(MultiLine = true), Category="Quest Infomation")
	FText QuestGoalContext;

	// 퀘스트 내용
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(MultiLine = true), Category="Quest Infomation")
	FText QuestContext;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(), Category="Quest Infomation")
	int32 DialogueRowId = INDEX_NONE;

	// 퀘스트 진행에 필요한 아이템
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(), Category="Quest Infomation | Required")
	TArray<FMQuestItem> RequiredItems;

	// 퀘스트 보상 아이템
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(), Category="Quest Infomation | Reward")
	TArray<FMQuestItem> RewardItems;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin = 0, ClampMax=2147483647), Category="Quest Infomation | Reward")
	int32 RewardGold = 0;
};
