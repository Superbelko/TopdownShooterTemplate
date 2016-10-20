// TopDownShooter project (c) 2016 V.Khmelevskiy

#pragma once

#include "GameFramework/GameMode.h"
#include "Runtime/AIModule/Classes/GenericTeamAgentInterface.h"
#include "TDLobbyGameMode.h"
#include "TDGameMode.generated.h"

// TODO: move these constants to more generic place
static FGenericTeamId TeamIdPlayer = { 0 };
static FGenericTeamId TeamIdMonster = { 1 };
static FGenericTeamId TeamIdNeutralToAll = { 100 };
static FGenericTeamId TeamIdFriendlyToAll = { 101 };
static FGenericTeamId TeamIdHostileToAll = { 102 };


class UTDSaveGameData;
class ATDPickUp;

/**
 * Main game mode class
 */
UCLASS()
class TOPDOWNSHOOTER_API ATDGameMode : public ATDLobbyGameMode
{
	GENERATED_BODY()
	
public:
	ATDGameMode(const FObjectInitializer& ObjectInitializer);

	// TODO: this should trigger countdown for restart from checkpoint
	UFUNCTION(BlueprintCallable, Category=Game)
	void OnPlayerKilled();

	FString GetLastSaveGameName() const;

	UFUNCTION(BlueprintCallable, Category=SaveGame)
	void CheckpointSaveGame(FString CheckpointActorName);

	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, class AActor* StartSpot) override;

public:
	/** Default dropped health pickup */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Drop)
	TSubclassOf<ATDPickUp> Drop_HealthPickup;

	/** Default dropped armor pickup */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Drop)
	TSubclassOf<ATDPickUp> Drop_ArmorPickup;

	/** Maps ammo type to drop item. Keep in sync with EAmmoType enum*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Drop)
	TArray<TSubclassOf<ATDPickUp>> Drop_AmmoPickups;

private:
	virtual void StartPlay() override;

};
