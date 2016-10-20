// TopDownShooter project (c) 2016 V.Khmelevskiy, S.Zhernovoy

#pragma once

#include "GameFramework/PlayerController.h"
#include "Runtime/AIModule/Classes/GenericTeamAgentInterface.h"
#include "TDPlayerController.generated.h"

class ATDPlayerCharacter;

/**
 * 
 */
UCLASS()
class TOPDOWNSHOOTER_API ATDPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:

	virtual FGenericTeamId GetGenericTeamId() const override;

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;


	virtual void UnPossess() override;

	void SetupCameraVolumeBlocking(UPrimitiveComponent* Component) const;


	virtual void PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel) override;

	UFUNCTION(BlueprintImplementableEvent, Category=Game)
	void OnPlayerKilled() const;


	virtual void OnRep_Pawn() override;


	virtual void SetPawn(APawn* InPawn) override;

protected:

	virtual void SetupInputComponent() override;
	virtual void PlayerTick(float DeltaSeconds) override;
	virtual void Possess(APawn* aPawn) override;


	// process local input
	virtual void OnPrimaryAttackStart();
	virtual void OnPrimaryAttackStop();
	void PrimaryAttack(bool bNewPrimaryAttack);

	UFUNCTION(unreliable, server, WithValidation)
	virtual void ServerOnPrimaryAttack(bool bNewPrimaryAttack);

	// process local input
	virtual void OnSecondaryAttackStart();
	virtual void OnSecondaryAttackStop();
	void SecondaryAttack(bool bNewPrimaryAttack);

	UFUNCTION(unreliable, server, WithValidation)
	virtual void ServerOnSecondaryAttack(bool bNewSecondaryAttack);

	void QuickLoad();

	void TraceMouse();

	bool PlayerIsAlive() const;

	/******************  Interface for hiding camera obstacles from player view ****************/ 
	UFUNCTION()
	void BeginCameraOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EndCameraOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


protected:
	UPROPERTY()
	ATDPlayerCharacter* MyPawn;

	FTimerHandle TraceTimer;

};
