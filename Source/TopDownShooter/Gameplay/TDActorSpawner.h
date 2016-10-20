// TopDownShooter project (c) 2016 V.Khmelevskiy

#pragma once

#include "GameFramework/Actor.h"
#include "TDActorSpawner.generated.h"


UCLASS(abstract)
class ATDActorSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATDActorSpawner(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category=Spawn)
	void Spawn();

public:

	/* Shows if actor was already spawned, by default do not spawn, spawn triggered in GameMode::BeginPlay */
	UPROPERTY(SaveGame, BlueprintReadWrite, EditAnywhere, Category = Spawn)
	bool bShouldSpawn = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Spawn)
	TSubclassOf<AActor> ActorClass;
};