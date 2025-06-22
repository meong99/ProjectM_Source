// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MViewportClient.h"
#include "DataAssets/PMAssetManager.h"
#include "WidgetRegister/MWidgetRegister.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "MWidgetInstanceList.h"
#include "UI/MWidgetBase.h"
#include "PMGameplayTags.h"
#include "UI/MWidgetLayout.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "InputKeyEventArgs.h"
#include "GameModes/MWorldSettings.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Level.h"

void UMViewportClient::Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice /*= true*/)
{
	Super::Init(WorldContext, OwningGameInstance, true);
}

void UMViewportClient::PostRender(UCanvas* Canvas)
{
	Super::PostRender(Canvas);

	if (bFading)
	{
		DrawScreenFade(Canvas);
	}
}

void UMViewportClient::ClearFade()
{
	bFading = false;
}

void UMViewportClient::Fade(const float Duration, const bool InbToBlack, const float InFadeDelay)
{
	if (World)
	{
		bFading = true;
		this->bToBlack = InbToBlack;
		FadeDuration = Duration;
		FadeStartTime = World->GetTimeSeconds();
		FadeDelay = InFadeDelay;
	}
}

bool UMViewportClient::InputKey(const FInputKeyEventArgs& EventArgs)
{
	if (GetWorld())
	{
		AMWorldSettings* WorldSetting = Cast<AMWorldSettings>(GetWorld()->GetWorldSettings());
		if (WorldSetting && WorldSetting->WorldTag == FPMGameplayTags::Get().Level_Lobby)
		{

		}
		else
		{
			if (EventArgs.Key == EKeys::Escape && EventArgs.Event == IE_Pressed)
			{
				OnPressEsc();
				return true;
			}
		}
	}

	return Super::InputKey(EventArgs);
}

bool UMViewportClient::WindowCloseRequested()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, true);
	// return true하면 패키징됐을 때 창이 먼저 꺼져서 정상적인 종료가 되지 않고, 백그라운드에 남아있음
	return false;
}

UMViewportClient* UMViewportClient::Get(const UObject* WorldContext)
{
	if (!WorldContext)
	{
		MCHAE_WARNING("WorldContextObject is null");
		return nullptr;
	}

	UWorld* World = WorldContext->GetWorld();
	if (World)
	{
		UGameInstance* GameInstance = World->GetGameInstance();
		if (GameInstance)
		{
			return Cast<UMViewportClient>(GameInstance->GetGameViewportClient());
		}
	}

	return nullptr;
}

void UMViewportClient::DrawScreenFade(UCanvas* Canvas)
{
	if (bFading)
	{
		if (World)
		{
			const float Time = World->GetTimeSeconds();
			const float Alpha = FMath::Clamp((Time - FadeStartTime - FadeDelay) / FadeDuration, 0.f, 1.f);

			if (Alpha == 1.f && !bToBlack)
			{
				bFading = false;
			}
			else
			{
				FColor OldColor = Canvas->DrawColor;
				FLinearColor FadeColor = FLinearColor::Black;
				FadeColor.A = bToBlack ? Alpha : 1 - Alpha;
				Canvas->DrawColor = FadeColor.ToFColor(true);
				Canvas->DrawTile(Canvas->DefaultTexture, 0, 0, Canvas->ClipX, Canvas->ClipY, 0, 0, Canvas->DefaultTexture->GetSizeX(), Canvas->DefaultTexture->GetSizeY());
				Canvas->DrawColor = OldColor;
			}
		}
	}
}

void UMViewportClient::LoadDefaultWidgetRegister()
{
	UPMAssetManager& AssetManager = UPMAssetManager::Get();
	check(UPMAssetManager::IsInitialized());

	FPrimaryAssetType		AssetType{ UMWidgetRegister::StaticClass()->GetFName() };
	TArray<FPrimaryAssetId>	PrimaryAssetIdList;
	AssetManager.GetPrimaryAssetIdList(AssetType, PrimaryAssetIdList);

	TArray<FName> LoadBundles;
	AssetManager.LoadPrimaryAssets(PrimaryAssetIdList, LoadBundles, FSimpleMulticastDelegate::FDelegate::CreateLambda(
		[this, PrimaryAssetIdList, &AssetManager]()->void
		{
			for (const FPrimaryAssetId& AssetId : PrimaryAssetIdList)
			{
				UMWidgetRegister* WidgetRegister = AssetManager.GetPrimaryAssetObject<UMWidgetRegister>(AssetId);

				if (IsValid(WidgetRegister) && WidgetRegister->RegisterTag.IsValid())
				{
					AddWidgetRegister(WidgetRegister->RegisterTag, WidgetRegister);
				}
				else
				{
					MCHAE_ERROR("WidgetRegistar or RegisterTag is not valid! Check!");
				}
			}

			ApplyWidgetLayout();
		})
	);
}

void UMViewportClient::AddWidgetRegister(const FGameplayTag& RegisterTag, UMWidgetRegister* InWidgetRegister)
{
	if (RegisterTag.IsValid() && InWidgetRegister)
	{
		RemoveWidgetRegister(RegisterTag);
		WidgetRegisterMap.Emplace(RegisterTag, InWidgetRegister);
	}
}

void UMViewportClient::RemoveWidgetRegister(const FGameplayTag& RegisterTag)
{
	UMWidgetRegister* Register = WidgetRegisterMap.FindRef(RegisterTag);
	if (Register)
	{
		for (const auto& Iter : Register->MappedWidgetData.WidgetData)
		{
			RemoveWidgetFromLayer(Iter.Key, true);
		}
	}
	WidgetRegisterMap.Remove(RegisterTag);
	WidgetInstanceListMap.Remove(RegisterTag);
}

void UMViewportClient::ClearLayer()
{
	if (WidgetLayout)
	{
		WidgetLayout->ClearAllWidget();
	}
}

UMWidgetInstanceList* UMViewportClient::CreateWidgetInRegister(const FGameplayTag& RegisterTag)
{
	UMWidgetInstanceList* WidgetInstanceList = WidgetInstanceListMap.FindRef(RegisterTag);
	if (WidgetInstanceList == nullptr)
	{
		WidgetInstanceList = CreateNewWidgetInstanceList(RegisterTag);
	}

	if (WidgetInstanceList)
	{
		WidgetInstanceList->CreateNewWidgets(WidgetRegisterMap.FindRef(RegisterTag));
	}

	return WidgetInstanceList;
}

void UMViewportClient::AddWidgetToViewport(const FGameplayTag& WidgetTag)
{
	UMWidgetBase* Widget = GetWidgetInstance(WidgetTag);

	if (Widget && !Widget->IsInViewport())
	{
		Widget->AddToViewport();
	}
}

void UMViewportClient::RemoveWidgetFromParent(const FGameplayTag& WidgetTag)
{
	UMWidgetBase* Widget = GetWidgetInstance(WidgetTag);

	if (Widget && Widget->IsInViewport())
	{
		Widget->RemoveFromParent();
	}
}

void UMViewportClient::OnPressEsc()
{
	FGameplayTag WidgetTag = FPMGameplayTags::Get().UI_Registry_Game_Option;
	UMWidgetBase* Widget = GetWidgetInstance(WidgetTag);

	if (WidgetLayout && WidgetLayout->RemoveTopWidgetInGameLayer())
	{
		return;
	}

	if (Widget && Widget->IsActivate())
	{
		RemoveWidgetFromLayer(WidgetTag);
	}
	else
	{
		AddWidgetToLayer(WidgetTag);
	}
}

UMWidgetBase* UMViewportClient::AddWidgetToLayer(const FGameplayTag& WidgetTag, const FMWidgetInfo& InWidgetInfo)
{
	UMWidgetBase* Widget = GetWidgetInstance(WidgetTag);
	if (WidgetLayout && Widget && !Widget->IsInLayer())
	{
		Widget->SetWidgetInfo(InWidgetInfo);
		if (!Widget->IsInitialized())
		{
			Widget->CallPreAddToLayer();
		}
		WidgetLayout->AddWidgetToLayer(Widget);
	}

	return Widget;
}

UMWidgetBase* UMViewportClient::RemoveWidgetFromLayer(const FGameplayTag& WidgetTag, const bool bIsDelete)
{
	UMWidgetBase* Widget = GetWidgetInstance(WidgetTag, bIsDelete);
	if (WidgetLayout && Widget)
	{
		if (Widget->IsInLayer())
		{
			WidgetLayout->RemoveWidgetFromLayer(GetWidgetInstance(WidgetTag));
			Widget->SetActivate(false);
		}
	}
	else
	{
		MCHAE_WARNING("WidgetLayout or WidgetInstance is null!");
	}

	return Widget;
}

UMWidgetBase* UMViewportClient::ToggleWidgetOnLayer(const FGameplayTag& WidgetTag, const FMWidgetInfo& InWidgetInfo)
{
	UMWidgetBase* Widget = GetWidgetInstance(WidgetTag);
	if (WidgetLayout && Widget)
	{
		if (Widget->IsInLayer())
		{
			RemoveWidgetFromLayer(WidgetTag);
		}
		else
		{
			AddWidgetToLayer(WidgetTag, InWidgetInfo);
		}
	}

	return Widget;
}

UMWidgetBase* UMViewportClient::GetWidgetInstance(const FGameplayTag& WidgetTag, const bool bIsDelete)
{
	if (bIsDelete)
	{
		return nullptr;
	}

	if (!WidgetTag.IsValid())
	{
		MCHAE_WARNING("RegisterTag or WidgetTag is not valid. WidgetTag = %s", *WidgetTag.ToString());
		return nullptr;
	}

	UMWidgetRegister* WidgetRegister = GetWidgetRegister(WidgetTag);

	if (WidgetRegister == nullptr)
	{
		MCHAE_WARNING("Can't Find WidgetRegister! Check WidgetTag's currect");
		return nullptr;
	}

	// WidgetInstanceList에서 Widget찾기
	UMWidgetBase* NewWidget = nullptr;
	UMWidgetInstanceList* WidgetInstanceList = WidgetInstanceListMap.FindRef(WidgetRegister->RegisterTag);
	if (WidgetInstanceList)
	{
		NewWidget = WidgetInstanceList->GetWidgetInstance(WidgetTag);
		if (NewWidget)
		{
			return NewWidget;
		}
	}
	else
	{
		// WidgetList가 없다면 만들기
		WidgetInstanceList = CreateNewWidgetInstanceList(WidgetRegister->RegisterTag);
	}

	// List에 Widget이 없어서 새로 만들고 추가 후 리턴
	NewWidget = WidgetInstanceList->CreateNewWidget(WidgetTag, WidgetRegister->GetWidgetClass(WidgetTag));

	return NewWidget;
}

UMWidgetInstanceList* UMViewportClient::CreateNewWidgetInstanceList(const FGameplayTag& RegisterTag)
{
	UMWidgetInstanceList* WidgetInstanceList = WidgetInstanceListMap.FindRef(RegisterTag);
	if (!WidgetInstanceList)
	{
		WidgetInstanceList = UMWidgetInstanceList::CreateNewWidgetInstanceList(this, RegisterTag);
		WidgetInstanceListMap.Add(RegisterTag, WidgetInstanceList);
	}

	return WidgetInstanceList;
}

UMWidgetRegister* UMViewportClient::GetWidgetRegister(const FGameplayTag& Tag)
{
	UMWidgetRegister* WidgetRegister = nullptr;
	FGameplayTagContainer ParentTags = Tag.GetGameplayTagParents();
	for (const FGameplayTag& ParentTag : ParentTags)
	{
		if (ParentTag.IsValid())
		{
			WidgetRegister = WidgetRegisterMap.FindRef(ParentTag);

			if (WidgetRegister)
			{
				break;
			}
		}
	}

	return WidgetRegister;
}

void UMViewportClient::ApplyWidgetLayout()
{
	if (WidgetLayout)
	{
		if (!WidgetLayout->IsInViewport())
		{
			WidgetLayout->AddToViewport();
		}
		return;
	}

	UMWidgetRegister* WidgetRegister = GetWidgetRegister(FPMGameplayTags::Get().UI_Registry_Layout_DefaultLayout.RequestDirectParent());

	WidgetLayout = Cast<UMWidgetLayout>(GetWidgetInstance(FPMGameplayTags::Get().UI_Registry_Layout_DefaultLayout));
	if (!WidgetLayout)
	{
		MCHAE_ERROR("Can't find DefaultLayout!!!!!!");
		check(false);
		return;
	}

	if (!WidgetLayout->IsInitialized())
	{
		WidgetLayout->PreAddToLayer();
	}

	WidgetLayout->AddToViewport();
}
