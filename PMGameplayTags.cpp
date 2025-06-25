#include "PMGameplayTags.h"
#include "GameplayTagsManager.h"

FPMGameplayTags FPMGameplayTags::GameplayTags;

void FPMGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
	GameplayTags.AddAllTags(Manager);
	// 태그를 ini로 Export하는 코드가 필요하다.
	// 그렇지 않으면 바이너리로 에디터를 열었을 때 런타임에 추가한 태그는 적용되지 않는다.
}

void FPMGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName), FString(TEXT("(ProjectMNativeTags) ")) + FString(TagComment));
}

void FPMGameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
	/*
	* Component Init State Tags
	*/
	AddTag(InitState_Spawned, "InitState.Spawned", "1: Actor/Component has initially spawned and can be extended");
	AddTag(InitState_DataAvailable, "InitState.DataAvailable", "2: All Required data has been loaded/replicated and is ready for initialize");
	AddTag(InitState_DataInitialized, "InitState.DataInitialized", "3: The available data has been initialized for this actor/component");
	AddTag(InitState_GameplayReady, "InitState.GameplayReady", "4: The actor/component is fully ready for active gameplay");


	/*
	* Input Tags
	*/
	AddTag(InputTag_Move, "InputTag.Move", "");
	AddTag(InputTag_Jump, "InputTag.Jump", "");
	AddTag(InputTag_Look_Mouse, "InputTag.Look.Mouse", "");
	AddTag(InputTag_Togle_Interaction, "InputTag.Togle.Interaction", "");
	AddTag(InputTag_Key_1, "InputTag.Key.1", "");
	AddTag(InputTag_Key_2, "InputTag.Key.2", "");
	AddTag(InputTag_Key_3, "InputTag.Key.3", "");
	AddTag(InputTag_Key_4, "InputTag.Key.4", "");

	/*
	* Widget Tags
	*/
	AddTag(UI_Registry_InputTag, "UI.Registry.InputTag", "");
	AddTag(UI_Registry_InputTag_Inventory, "UI.Registry.InputTag.Inventory", "");
	AddTag(UI_Registry_InputTag_PlayerQuest, "UI.Registry.InputTag.PlayerQuest", "");

	AddTag(UI_Registry_Layout_DefaultLayout, "UI.Registry.Layout.DefaultLayout", "");
	AddTag(UI_Registry_HUD_MainHUD, "UI.Registry.HUD.MainHUD", "각 게임 모드 별 HUD. 등록한다면 Character가 초기화된 후 자동으로 위젯이 켜진다");
	AddTag(UI_Registry_Game_Option, "UI.Registry.Game.Option", "");
	
	AddTag(UI_Game_NameWidget, "UI.Game.NameWidget", "");

	/*
	* Item
	*/
	AddTag(Item_Quentity, "Item.Quentity", "Item의 수량");

	/*
	* GameplayEffects
	*/
	AddTag(Ability_Effect_SetByCaller_Health, "Ability.Effect.SetByCaller.Health", "");
	AddTag(Ability_Effect_SetByCaller_MaxHealth, "Ability.Effect.SetByCaller.MaxHealth", "");
	AddTag(Ability_Effect_SetByCaller_DefensePower, "Ability.Effect.SetByCaller.DefensePower", "");
	AddTag(Ability_Effect_SetByCaller_AttackPower, "Ability.Effect.SetByCaller.AttackPower", "");
	AddTag(Ability_Effect_SetByCaller_HealthRecovery, "Ability.Effect.SetByCaller.HealthRecovery", "");

	/*
	* Gameplay
	*/
	AddTag(UI_Registry_Game_Dialogue, "UI.Registry.Game.Dialogue", "");
	AddTag(UI_Registry_Game_ItemContext, "UI.Registry.Game.ItemContext", "");
	AddTag(UI_Registry_Game_InteractionList, "UI.Registry.Game.InteractionList", "");
	AddTag(UI_Registry_Game_Shop, "UI.Registry.Game.Shop", "");
	AddTag(UI_Registry_Game_Restart, "UI.Registry.Game.Restart", "");
	AddTag(UI_Registry_Game_Loading, "UI.Registry.Game.Loading", "");

	/*
	* WorldTag
	*/
	AddTag(Level_Persistent, "Level.Persistent", "");
	AddTag(Level_Persistent_MainLevel, "Level.Persistent.MainLevel", "");
	AddTag(Level_Square, "Level.Square", "");
	AddTag(Level_Field, "Level.Field", "");
	AddTag(Level_Lobby, "Level.Lobby", "");

	/*
	* GameplayEvent
	*/
	AddTag(GameplayEvent_Character_State_Dead, "GameplayEvent.Character.State.Dead", "");
	AddTag(GameplayEvent_Character_ChangeExp, "GameplayEvent.Character.ChangeExp", "");
}