// TopDownShooter project (c) 2016 V.Khmelevskiy

#pragma once

#include "GameFramework/Actor.h"
#include "TDPickUp.generated.h"

class ATDPlayerCharacter;
class UTDItem;

UCLASS(abstract)
class TOPDOWNSHOOTER_API ATDPickUp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATDPickUp(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UFUNCTION()
	virtual void OnPickUp(ATDPlayerCharacter* Player);

	void RemovePickUp();

	/** Set larger radius, adjust collision so it visible only to and may be picked up by specific player */
	UFUNCTION()
	void MakePersonal(ATDPlayerCharacter* OwnerPlayer);

	UFUNCTION()
	void OnRep_IsPersonal();
public:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, category = Config)
	UParticleSystemComponent* ProjectileFX;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, category = Config)
	USphereComponent* Sphere;


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(ReplicatedUsing=OnRep_IsPersonal, BlueprintReadOnly, VisibleAnywhere, category = PickUp)
	bool bIsPersonal;
};

UCLASS()
class TOPDOWNSHOOTER_API ATDHealthPickUp : public ATDPickUp
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
/*	ATDHealthPickUp(const FObjectInitializer& ObjectInitializer);*/

	virtual void OnPickUp(ATDPlayerCharacter* Player) override;

public:

	UPROPERTY(EditDefaultsOnly, category = PickUp)
	float Health;

};

UCLASS()
class TOPDOWNSHOOTER_API ATDArmorPickUp : public ATDPickUp
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	/*	ATDHealthPickUp(const FObjectInitializer& ObjectInitializer);*/

	virtual void OnPickUp(ATDPlayerCharacter* Player) override;

public:

	UPROPERTY(EditDefaultsOnly, category = PickUp)
	float Armor;

};

UCLASS()
class TOPDOWNSHOOTER_API ATDItemPickUp : public ATDPickUp
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	/*	ATDHealthPickUp(const FObjectInitializer& ObjectInitializer);*/

	virtual void OnPickUp(ATDPlayerCharacter* Player) override;

public:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, category = PickUp)
	TSubclassOf<UTDItem> Item;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, category = PickUp)
	int32 ItemCount = 1;

};