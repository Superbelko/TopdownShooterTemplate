// TopDownShooter project (c) 2016 V.Khmelevskiy

#pragma once

#include "GameFramework/GameMode.h"
#include "TDLobbyGameMode.h"
#include "TDGameInstance.generated.h"

class UTDSaveGameData;

struct FTDLoadingScreenBrush : public FSlateDynamicImageBrush, public FGCObject
{
	FTDLoadingScreenBrush(const FName InTextureName, const FVector2D& InImageSize)
		: FSlateDynamicImageBrush(InTextureName, InImageSize)
	{
		ResourceObject = LoadObject<UObject>(NULL, *InTextureName.ToString());
	}

	virtual void AddReferencedObjects(FReferenceCollector& Collector)
	{
		if (ResourceObject)
		{
			Collector.AddReferencedObject(ResourceObject);
		}
	}
};

class STDLoadingScreen : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STDLoadingScreen) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	EVisibility GetLoadIndicatorVisibility() const
	{
		return EVisibility::Visible;
	}

	/** loading screen image brush */
	TSharedPtr<FSlateDynamicImageBrush> LoadingScreenBrush;
};

UCLASS()
class UTDGameIntance : public UGameInstance
{
	GENERATED_BODY()

public:

	void ShowLoadingScreen();

	UFUNCTION(BlueprintCallable, Category=LoadingScreen)
	void HideLoadingScreen();

	virtual void StartGameInstance() override;

	virtual void Init() override;

public:


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Game)
	FString LoadGameFile;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Game)
	FString LastSavedGame;

	UPROPERTY()
	UTDSaveGameData* LoadedSaveGame;

protected:

	void OnPreLoadMap(const FString& arg);
	void OnPostLoadMap();

protected:
	TSharedPtr<STDLoadingScreen> LoadingScreenWidget;

	bool bLoadingWidgetVisible;
};