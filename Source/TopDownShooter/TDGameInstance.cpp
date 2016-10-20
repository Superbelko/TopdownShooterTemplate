// TopDownShooter project (c) 2016 V.Khmelevskiy

#include "TopDownShooter.h"

#include "TDGameInstance.h"
#include "TDLoadingScreen/TDLoadingScreen.h"

// needed to reset drop data
#include "AI/TDMonsterCharacter.h"

// loading screen
#include "SSafeZone.h"
#include "SThrobber.h"



void STDLoadingScreen::Construct(const FArguments& InArgs)
{
	static const FName LoadingScreenName(TEXT("/Game/UI/Menu/LoadingScreen.LoadingScreen"));

	//since we are not using game styles here, just load one image
	LoadingScreenBrush = MakeShareable(new FTDLoadingScreenBrush(LoadingScreenName, FVector2D(1920, 1080)));

	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SImage)
			.Image(LoadingScreenBrush.Get())
		]
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SSafeZone)
			.VAlign(VAlign_Bottom)
			.HAlign(HAlign_Right)
			.Padding(10.0f)
			.IsTitleSafe(true)
			[
				SNew(SThrobber)
				.Visibility(this, &STDLoadingScreen::GetLoadIndicatorVisibility)
			]
		]
	];
}


// TODO: pass level URI to load level specific image
void UTDGameIntance::ShowLoadingScreen()
{
	
	ITDLoadingScreenModule* const LoadingScreenModule = FModuleManager::LoadModulePtr<ITDLoadingScreenModule>("TDLoadingScreen");
	if (LoadingScreenModule != nullptr)
	{
		LoadingScreenModule->StartInGameLoadingScreen();
	}

	if (LoadingScreenWidget.IsValid())
		return;

	if (bLoadingWidgetVisible)
		return;

	LoadingScreenWidget = SNew(STDLoadingScreen);

	GEngine->GameViewport->AddViewportWidgetContent(LoadingScreenWidget.ToSharedRef(), 1000); // zorder 1000, high enough to be on top
	bLoadingWidgetVisible = true;
}

void UTDGameIntance::HideLoadingScreen()
{
	if (!LoadingScreenWidget.IsValid())
		return;

	/*
	if (!bLoadingWidgetVisible)
		return;
	*/

	// Make sure we hide the loading screen when the level is done loading
	UGameViewportClient * Viewport = GetGameViewportClient();

	if (Viewport)
	{
		Viewport->RemoveAllViewportWidgets();
		//Viewport->RemoveViewportWidgetContent(LoadingScreenWidget.ToSharedRef());
		bLoadingWidgetVisible = false;
	}
}


void UTDGameIntance::StartGameInstance()
{
	Super::StartGameInstance();
	// FIX ME: loading screen had some issues when starting game, skip it for now
	//ShowLoadingScreen();
}

void UTDGameIntance::Init()
{
	Super::Init();

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UTDGameIntance::OnPreLoadMap);
	FCoreUObjectDelegates::PostLoadMap.AddUObject(this, &UTDGameIntance::OnPostLoadMap);
}

void UTDGameIntance::OnPreLoadMap(const FString& arg)
{
	ShowLoadingScreen();
	ATDMonsterCharacter::ResetDropData();
}

void UTDGameIntance::OnPostLoadMap()
{
	HideLoadingScreen();
}
