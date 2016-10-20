// TopDownShooter project (c) 2016 V.Khmelevskiy

#pragma once

#include "Object.h"
#include "Archive.h"
#include "TDSaveGame.generated.h"

class ATDActorSpawner;


/**
* Marker interface used for searching and serializing actors
*/
UINTERFACE()
class UTDSaveableInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};


// Marker interface
class ITDSaveableInterface
{
	GENERATED_IINTERFACE_BODY()

public:

};


/**
* Aux class for quick load functionality
*/
UCLASS()
class UTDSaveGameInfo : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite,EditAnywhere, Category=Save)
	FString LastSavedFile;
};


/**
* SaveGame archiver
*/
struct FTDSaveGameArchive : public FObjectAndNameAsStringProxyArchive
{
	FTDSaveGameArchive(FArchive& InInnerArchive);
};


/**
* Basic info about actor stored in save game file alongside with data
*/
USTRUCT()
struct FActorData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	TSubclassOf<AActor> Class;

	UPROPERTY(SaveGame)
	FTransform Transform;

	UPROPERTY(SaveGame)
	FString Name;

	UPROPERTY(SaveGame)
	TArray<uint8> Data;
};


/**
 * SaveGame data model class
 */
UCLASS()
class TOPDOWNSHOOTER_API UTDSaveGameData : public USaveGame
{
	GENERATED_BODY()

public:

	UTDSaveGameData(const class FObjectInitializer& ObjectInitializer);

	// (checkpoint) actor name to spawn at 
	UPROPERTY()
	FString PlayerSpawnPoint;

	/** Level name relative to /Game/Maps folder */
	// TODO: should be full level path? no reason to enforce this, UWorld has level URI we can store here which is also should be safer
	UPROPERTY()
	FString LevelName;

	UPROPERTY()
	TArray<uint8> LevelData;

	// ================== PLAYER ==================
	UPROPERTY()
	int32 PlayerNum;

	// used for differentiation between players, assign player character by name
	// TODO: do proper mapping based on FUniqueNetId
	UPROPERTY()
	TArray<FString> PlayerNames;

	UPROPERTY()
	TArray<FActorData> PlayerRecords;

	// ================== CHECKPOINT AND SPAWNERS ==================

	// List of active spawners
	UPROPERTY()
	TArray<FString> EnabledSpawners;

	// List of not yet activated checkpoints
	UPROPERTY()
	TArray<FString> ActiveCheckpoints;

	// ================== ACTORS ==================

	UPROPERTY()
	int32 ActorsNum;

	UPROPERTY()
	TArray<FActorData> ActorRecords;
};
