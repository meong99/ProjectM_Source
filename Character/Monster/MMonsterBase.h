#pragma once

#include "Character/MCharacterBase.h"
#include "Character/Monster/MMonsterTypes.h"
#include "AbilitySystem/PMAbilitySet.h"
#include "GameplayEffectTypes.h"
#include "MMonsterSpawner.h"
#include "MMonsterBase.generated.h"

class UMInteractionComponent;
class UPMAbilitySystemComponent;
class UPMHealthComponent;
class UPMHealthSet;
class UMMonsterDefinition;
class AMMonsterSpawner;
class UMMonsterTradeComponent;
class UPMCombatSet;
class UWidgetComponent;

UCLASS(Blueprintable, BlueprintType)
class PROJECTM_API AMMonsterBase : public AMCharacterBase
{
	GENERATED_BODY()

/*
* Overrided Function
*/
public:
	AMMonsterBase(const FObjectInitializer& ObjectInitializer);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void InitCharacterName() override;

	virtual void OnDead() override;
	virtual void Destroyed() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/*
* Member Functions
*/
public:
	// Beginplay이전에 호출
	UFUNCTION(BlueprintCallable)
	void SetSpawner(AMMonsterSpawner* InSpawner);

	UFUNCTION(BlueprintCallable)
	virtual UPMAbilitySystemComponent* GetMAbilitySystemComponent() const override;

	UMMonsterDefinition*	GetMonsterDefinition() { return MonsterDefinition; }
	const int32				GetMonsterRowId() const { return MonsterRowId; }
	AActor*					GetMonsterSpawner() const { return MonsterSpawner.Get(); }

#if WITH_EDITOR
	void	SetMonsterRowId(const int32 InRowId) { MonsterRowId = InRowId; }
#endif
protected:
	void Callback_OnDamaged(const FOnAttributeChangeData& ChangeData);
	void GiveRewardToPlayer();
	void RequestRemoveToSpawner();
/*
* Member Variables
*/
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (), Category = "ProjectM")
	int32 MonsterRowId = INDEX_NONE;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Monster")
	TObjectPtr<UWidgetComponent> DamageWidgetComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster")
	TObjectPtr<UPMAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster")
	TObjectPtr<UMInteractionComponent> InteractionComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster")
	TObjectPtr<UMMonsterTradeComponent> MonsterTradeComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Monster")
	UMMonsterDefinition* MonsterDefinition;

	UPROPERTY(BlueprintReadOnly, Category = "Monster")
	TObjectPtr<UPMHealthSet> HealthSet;

	UPROPERTY(BlueprintReadOnly, Category = "Monster")
	TObjectPtr<UPMCombatSet> CombatSet;

	UPROPERTY(BlueprintReadOnly, Category = "Monster")
	AActor* LastAttacker;

	UPROPERTY()
	TMap<int32, FMAbilitySet_GrantedHandles> GrantedHandles;

	UPROPERTY()
	FMAbilitySet_AppliedEffectHandles AppliedDefaultEffects;

	UPROPERTY()
	TObjectPtr<AMMonsterSpawner> MonsterSpawner;
};