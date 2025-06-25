#pragma once

#include "Containers/UnrealString.h"
#include "Containers/Map.h"
#include "GameplayTagContainer.h"

class UGameplayTagsManager;

struct FPMGameplayTags
{
	static const FPMGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeTags();

	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);
	void AddAllTags(UGameplayTagsManager& Manager);

	/*
	* Component Init State Tags
	*/
	FGameplayTag InitState_Spawned;
	FGameplayTag InitState_DataAvailable;
	FGameplayTag InitState_DataInitialized;
	FGameplayTag InitState_GameplayReady;

	/*
	* Input Tags
	*/
	FGameplayTag InputTag_Move;
	FGameplayTag InputTag_Jump;
	FGameplayTag InputTag_Look_Mouse;
	FGameplayTag InputTag_Togle_Interaction;
	FGameplayTag InputTag_Key_1;
	FGameplayTag InputTag_Key_2;
	FGameplayTag InputTag_Key_3;
	FGameplayTag InputTag_Key_4;

	/*
	* Widget Tags
	*/
	FGameplayTag UI_Registry_InputTag;
	FGameplayTag UI_Registry_InputTag_Inventory;
	FGameplayTag UI_Registry_InputTag_PlayerQuest;

	FGameplayTag UI_Registry_Layout_DefaultLayout;
	FGameplayTag UI_Registry_HUD_MainHUD;
	FGameplayTag UI_Registry_Game_Option;

	FGameplayTag UI_Game_NameWidget;
	/*
	* Item
	*/
	FGameplayTag Item_Quentity;

	/*
	* GameplayEffects
	*/
	FGameplayTag Ability_Effect_SetByCaller_Health;
	FGameplayTag Ability_Effect_SetByCaller_MaxHealth;
	FGameplayTag Ability_Effect_SetByCaller_DefensePower;
	FGameplayTag Ability_Effect_SetByCaller_AttackPower;
	FGameplayTag Ability_Effect_SetByCaller_HealthRecovery;

	/*
	* Gameplay
	*/
	FGameplayTag UI_Registry_Game_Dialogue;
	FGameplayTag UI_Registry_Game_InteractionList;
	FGameplayTag UI_Registry_Game_Shop;
	FGameplayTag UI_Registry_Game_Restart;
	FGameplayTag UI_Registry_Game_ItemContext;
	FGameplayTag UI_Registry_Game_Loading;
	

	/*
	* WorldTag
	*/
	FGameplayTag Level_Persistent;
	FGameplayTag Level_Persistent_MainLevel;
	FGameplayTag Level_Square;
	FGameplayTag Level_Field;
	FGameplayTag Level_Lobby;

	/*
	* GameplayEvent
	*/
	FGameplayTag GameplayEvent_Character_State_Dead;
	FGameplayTag GameplayEvent_Character_ChangeExp;

private:
	static FPMGameplayTags GameplayTags;
};