// TopDownShooter project (c) 2016 V.Khmelevskiy

#include "TopDownShooter.h"
#include "Objective.h"


// Sets default values
AObjective::AObjective(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	RootComponent = Billboard;
}

void AObjective::SetIsActive(bool bNewActive, EObjectiveStatus Status /*= EObjectiveStatus::Undefined*/)
{
	if (bNewActive)
	{
		OnBecomeActive();
	}
	else // !bNewActive
	{
		OnBecomeInactive(Status);
	}
}

