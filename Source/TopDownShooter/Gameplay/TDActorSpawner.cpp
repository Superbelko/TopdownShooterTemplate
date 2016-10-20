// TopDownShooter project (c) 2016 V.Khmelevskiy

#include "TopDownShooter.h"
#include "TDActorSpawner.h"


// Sets default values
ATDActorSpawner::ATDActorSpawner(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATDActorSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (bShouldSpawn)
		Spawn();
}

void ATDActorSpawner::Spawn()
{
	if (ActorClass)
	{
		GetWorld()->SpawnActor<AActor>(ActorClass, GetTransform());
	}

	bShouldSpawn = false;
}
