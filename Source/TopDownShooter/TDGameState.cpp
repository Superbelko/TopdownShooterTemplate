// TopDownShooter project (c) 2016 V.Khmelevskiy

#include "TopDownShooter.h"
#include "TDGameState.h"
#include "Player/TDPlayerController.h"
#include "Gameplay/Objective.h"

#include "UnrealNetwork.h"



void ATDGameState::OnRep_PlayerKilled()
{
	ATDPlayerController* PlayerController = Cast<ATDPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PlayerController)
	{
		PlayerController->OnPlayerKilled();
	}
}

void ATDGameState::OnRep_ObjectiveChanged()
{
	SetObjective(CurrentObjective);
}

void ATDGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATDGameState, bPlayerKilled);
	DOREPLIFETIME(ATDGameState, CurrentObjective);
}

FString ATDGameState::GetValueString(const FString& Key) const
{
	if (!LevelData.Contains(Key))
		return FString();

	return LevelData[Key];
}

void ATDGameState::SetValueString(const FString& Key, const FString& Data)
{
	if (!LevelData.Contains(Key))
	{ 
		LevelData.Emplace(Key, Data);
	}
	else
	{
		LevelData[Key] = Data;
	}
}

void ATDGameState::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);


	if (Ar.ArIsSaveGame)
	{
		Ar << CurrentObjective;

		Ar << LevelData;

		Ar << Objectives;

		// mark objective active
		if (Ar.ArIsLoading && CurrentObjective)
		{
			SetObjective(CurrentObjective);
		}
	}
}

void ATDGameState::SetObjective(AObjective* NewObjective)
{
	if (!NewObjective)
	{
		CurrentObjective = nullptr;

		for (AObjective* objective : Objectives)
		{
			objective->SetIsActive(false);
		}

		return;
	}

	Objectives.AddUnique(NewObjective);

	CurrentObjective = NewObjective;

	NewObjective->SetIsActive(true);
}
