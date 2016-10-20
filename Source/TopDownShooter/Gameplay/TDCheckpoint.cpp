// TopDownShooter project (c) 2016 V.Khmelevskiy

#include "TopDownShooter.h"
#include "TDCheckpoint.h"
#include "TDGameMode.h"

// Sets default values
ATDCheckpoint::ATDCheckpoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ATDCheckpoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATDCheckpoint::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ATDCheckpoint::TriggerSave()
{
	if (HasAuthority())
	{
		ATDGameMode* GameMode = Cast<ATDGameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{
			bCheckpointActive = false;
			GameMode->CheckpointSaveGame(GetName());
		}
	}
}

