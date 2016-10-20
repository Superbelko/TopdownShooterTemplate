// TopDownShooter project (c) 2016 V.Khmelevskiy

#include "TopDownShooter.h"

#include "Archive.h"
#include "FileManager.h"
#include "Engine/TriggerBase.h"
#include "Runtime/Core/Public/Serialization/Archive.h"

#include "TDGameMode.h"
#include "TDSaveGame.h"
#include "TDGameState.h"
#include "TDGameInstance.h"
#include "AI/TDMonsterCharacter.h"
#include "Gameplay/TDActorSpawner.h"
#include "Gameplay/TDCheckpoint.h"
#include "Player/TDPlayerState.h"
#include "Player/TDPlayerCharacter.h"
#include "Player/TDPlayerController.h"


ATDGameMode::ATDGameMode(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PlayerStateClass = ATDPlayerState::StaticClass();
}


void ATDGameMode::OnPlayerKilled()
{
	ATDGameState* GameState = GetGameState<ATDGameState>();
	if (GameState)
	{
		GameState->bPlayerKilled = true;
	}
}


FString ATDGameMode::GetLastSaveGameName() const
{
	FString Result;
	UTDGameIntance* GameInstance = Cast<UTDGameIntance>(GetGameInstance());
	if (GameInstance)
	{
		UTDSaveGameInfo* SaveInfo = Cast<UTDSaveGameInfo>(
			UGameplayStatics::LoadGameFromSlot(GameInstance->LastSavedGame, 0)
			);

		if (SaveInfo)
		{
			Result = GameInstance->LoadGameFile = SaveInfo->LastSavedFile;
		}
	}

	return Result;
}

// saves game related actors (that has saveable interface, but also specific types of actors)
// iterates through all actors of that type, serializes necessary data and store all in same place (actor class, name and data itself)
void ATDGameMode::CheckpointSaveGame(FString CheckpointActorName)
{
	TArray<AActor*> Actors;
	TArray<AActor*> Spawners;
	TArray<AActor*> Checkpoints;

	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UTDSaveableInterface::StaticClass(), Actors);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATDActorSpawner::StaticClass(), Spawners);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATDCheckpoint::StaticClass(), Checkpoints);


	UTDSaveGameData* SaveData = Cast<UTDSaveGameData>(UGameplayStatics::CreateSaveGameObject(UTDSaveGameData::StaticClass()));
	
	SaveData->LevelName = GetWorld()->GetMapName();
	SaveData->PlayerSpawnPoint = CheckpointActorName;


	/* ==================== PLAYERS ====================== */

	{
		// currently stores order dependent list, so players should join the same order all the time to get right character,
		// need to assign using unique player id, but since we don't use any online subsystem there is no such term

		TArray<AActor*> Players;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATDPlayerCharacter::StaticClass(), Players);
		int i = 0;
		for (auto* Player : Players)
		{
			FMemoryWriter PlayerWriter(SaveData->PlayerRecords[i].Data);
			FTDSaveGameArchive PlayerAr(PlayerWriter);
			FActorData ActorData;
			ActorData.Class = Player->GetClass();
			ActorData.Transform = Player->GetTransform();
			ActorData.Name = Player->GetName();
			SaveData->PlayerRecords[i] = ActorData;

			Player->Serialize(PlayerAr);
			PlayerWriter.FlushCache();
			PlayerAr.Flush();
			i++;
		}
	}


	/* ==================== SPAWNERS ====================== */

	for (const auto& Spawner : Spawners)
	{
		auto spawner = Cast<ATDActorSpawner>(Spawner);
		if (spawner->bShouldSpawn)
			SaveData->EnabledSpawners.Add(spawner->GetName());
	}


	/* ==================== CHECKPOINTS ====================== */

	for (const auto& Checkpoint : Checkpoints)
	{
		auto checkpoint = Cast<ATDCheckpoint>(Checkpoint);
		if (checkpoint->bCheckpointActive)
			SaveData->ActiveCheckpoints.Add(checkpoint->GetName());
	}


	/* ==================== MONSTERS ====================== */
	{
		// filter dead monsters away
		TArray<AActor*> FilteredActors = Actors.FilterByPredicate(
			[&](AActor* Actor) -> bool
			{ 
				ATDMonsterCharacter* Monster = Cast<ATDMonsterCharacter>(Actor);
				if (!Monster)
					return true;
				return !(Monster->IsDead());
			}
		);

		// store alive monsters count
		SaveData->ActorsNum = FilteredActors.Num();
		SaveData->ActorRecords.Reset();
		SaveData->ActorRecords.AddDefaulted(SaveData->ActorsNum);

		int i = 0;
		for (const auto& Actor : FilteredActors)
		{
			FMemoryWriter SaveableMemoryWriter(SaveData->ActorRecords[i].Data);
			FTDSaveGameArchive SaveableAr(SaveableMemoryWriter);

			SaveData->ActorRecords[i].Class = Actor->GetClass();
			SaveData->ActorRecords[i].Transform = Actor->GetTransform();
			SaveData->ActorRecords[i].Name = Actor->GetName();

			// serialize the object
			Actor->Serialize(SaveableAr);
			SaveableMemoryWriter.FlushCache();
			SaveableAr.Flush();
			i++;
		}
	}


	/* ==================== LEVEL DATA ====================== */

	ATDGameState* _GameState = GetGameState<ATDGameState>();
	if (_GameState)
	{
		FMemoryWriter LevelMemoryWriter(SaveData->LevelData);
		FTDSaveGameArchive SaveableAr(LevelMemoryWriter);

		_GameState->Serialize(SaveableAr);
		LevelMemoryWriter.FlushCache();
		SaveableAr.Flush();
	}



	UGameplayStatics::SaveGameToSlot(SaveData, "checkpoint", 0);
	UTDGameIntance* GameInstance = Cast<UTDGameIntance>(GetGameInstance());
	GameInstance->LastSavedGame = "checkpoint";
	GameInstance->LoadGameFile = "checkpoint";
	GameInstance->LoadedSaveGame = SaveData;
}


APawn* ATDGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, class AActor* StartSpot)
{
	UTDGameIntance* GameInstance = Cast<UTDGameIntance>(GetGameInstance());
	if (GameInstance && GameInstance->LoadedSaveGame)
	{
		UTDSaveGameData* SaveData = GameInstance->LoadedSaveGame;
		TArray<AActor*> Checkpoints;
		TArray<AActor*> Players;
		
#if 0
		// this is better (performance-wise) way of handling player death then checking if is dead, but for some reason it doesn't reseted on game load
		ATDPlayerController* PlayerCon = Cast<ATDPlayerController>(NewPlayer);
		if (PlayerCon)
		{
			PlayerCon->SetCinematicMode(false, false, false);
		}
#endif

		//====================================
		// Find checkpoint

		ATDGameState* GameState = GetGameState<ATDGameState>();
		if (GameInstance->LoadedSaveGame->PlayerSpawnPoint.Len() > 0)
		{
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATDCheckpoint::StaticClass(), Checkpoints);
			for (AActor* Actor : Checkpoints)
			{
				if (Actor->GetName() == GameInstance->LoadedSaveGame->PlayerSpawnPoint)
				{
					GameState->CheckPoint = Actor;
					break;
				}
			}
		}

		//====================================
		// Spawn players

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController::StaticClass(), Players);

		int i = 0;
		for (auto* Player : Players)
		{
			// FIX ME: can cause issues when same player joined next time will get default pawn
			if (Player == NewPlayer)
			{
				APlayerController* PlayerController = Cast<APlayerController>(Player);
				if (PlayerController)
				{
					FMemoryReader PlayerReader(SaveData->PlayerRecords[i].Data);
					FTDSaveGameArchive PlayerAr(PlayerReader);

					// spawn default pawn if no saved data
					if (!SaveData->PlayerRecords[i].Class)
						break;

					// choose checkpoint transform, or get player start
					FTransform Transform = GameState->CheckPoint ? GameState->CheckPoint->GetTransform() : FindPlayerStart(NewPlayer)->GetActorTransform();

					AActor* SpawnedActor = UGameplayStatics::BeginDeferredActorSpawnFromClass(this, SaveData->PlayerRecords[i].Class, Transform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
					SpawnedActor->Serialize(PlayerAr);
					UGameplayStatics::FinishSpawningActor(SpawnedActor, Transform);

					return Cast<APawn>(SpawnedActor);
				}
			}
			i++;
		}

		// if we don't match player at least make sure default character spawned at checkpoint 
		return Super::SpawnDefaultPawnFor_Implementation(NewPlayer, GameState->CheckPoint);
	}

	return Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);
}


void ATDGameMode::StartPlay()
{
	UTDGameIntance* GameInstance = Cast<UTDGameIntance>(GetGameInstance());
	if (GameInstance && GameInstance->LoadedSaveGame)
	{
		UTDSaveGameData* SaveData = GameInstance->LoadedSaveGame;

		TArray<AActor*> Spawners;
		TArray<AActor*> Checkpoints;
		int32 unused;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATDActorSpawner::StaticClass(), Spawners);
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATDCheckpoint::StaticClass(), Checkpoints);

		/* ==================== SPAWNERS ====================== */

		// disable any spawners that should not spawn
		for (AActor* Spawner : Spawners)
		{
			if (!GameInstance->LoadedSaveGame->EnabledSpawners.Find(Spawner->GetName(), unused))
			{
				Cast<ATDActorSpawner>(Spawner)->bShouldSpawn = false;
			}
		}

		/* ==================== CHECKPOINTS ====================== */

		// disable non active checkpoints
		for (AActor* Checkpoint : Checkpoints)
		{
			if (!GameInstance->LoadedSaveGame->ActiveCheckpoints.Find(Checkpoint->GetName(), unused))
			{
				Cast<ATDCheckpoint>(Checkpoint)->bCheckpointActive = false;
			}
		}

		/* ==================== MONSTERS ====================== */

		// load actors from save file
		{
			// iterate actors
			for (const auto& Entry : SaveData->ActorRecords)
			{
				FMemoryReader MemoryReader(Entry.Data);
				FTDSaveGameArchive Ar(MemoryReader);

				AActor* SpawnedActor = UGameplayStatics::BeginDeferredActorSpawnFromClass(this, Entry.Class, Entry.Transform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

				SpawnedActor->Serialize(Ar);

				UGameplayStatics::FinishSpawningActor(SpawnedActor, Entry.Transform);
			}
		}

		/* ==================== LEVEL DATA ====================== */

		ATDGameState* _GameState = GetGameState<ATDGameState>();
		if (_GameState)
		{
			// check if we have any data stored
			if (SaveData->LevelData.Num())
			{
				FMemoryReader LevelMemoryReader(SaveData->LevelData);
				FTDSaveGameArchive ReaderAr(LevelMemoryReader);

				_GameState->Serialize(ReaderAr);
			}
		}
	}

	// init & beginplay all actors
	Super::StartPlay();
}
