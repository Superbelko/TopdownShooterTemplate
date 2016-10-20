// TopDownShooter project (c) 2016 V.Khmelevskiy

#pragma once

#include "GameFramework/Actor.h"
#include "TDCheckpoint.generated.h"

UCLASS()
class TOPDOWNSHOOTER_API ATDCheckpoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATDCheckpoint();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/* Save game progress and disables this checkpoint */
	UFUNCTION(BlueprintCallable, Category = Checkpoint)
	void TriggerSave();

public:


	/* Should checkpoint trigger save game? */
	UPROPERTY(SaveGame, BlueprintReadWrite, EditAnywhere, Category = Spawn)
	bool bCheckpointActive = true;
	
};
