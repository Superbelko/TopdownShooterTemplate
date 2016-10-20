// TopDownShooter project (c) 2016 V.Khmelevskiy

#pragma once

#include "GameFramework/Character.h"
#include "Runtime/AIModule/Classes/GenericTeamAgentInterface.h"
#include "TDCharacter.generated.h"

class ATDProjectileBase;

UCLASS(Abstract)
class TOPDOWNSHOOTER_API ATDCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATDCharacter(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnDeath();

	UFUNCTION()
	virtual void Die();

	void AddHealth(float HealthToAdd);
	bool IsFullHealth();

	UFUNCTION(BlueprintCallable, Category=Stats)
	float GetMaxHealth() const;

	UFUNCTION(BlueprintPure, Category = Health)
	bool IsDead() const;

	virtual FGenericTeamId GetGenericTeamId() const override;

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	UFUNCTION(BlueprintCallable, Category = Attack)
	virtual void FireProjectile(TSubclassOf<ATDProjectileBase> ProjClass, FName SocketName, FVector TargetLoc, float Damage) const;


	UFUNCTION(BlueprintCallable, Category = Attack)
	FVector GetPredictionLocation(const AActor* Target, float Velocity);

	UFUNCTION(BlueprintCallable, Category = Movement)
	FORCEINLINE float GetRunningSpeedModifier() const { return RunningSpeedModifier;  }

	UFUNCTION(BlueprintCallable, Category = Movement)
	FORCEINLINE bool IsRunning() const { return bIsRunning; }

	// TODO: move to more generic location
	UFUNCTION()
	void SpawnImpactEffects(const FHitResult& Impact, TSubclassOf<ATDImpactEffect> ImpactTemplate);

	/** Returns true if health ratio lower than LowHealthRatio */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Stats)
	bool IsLowHealth() const;

#if 0
	virtual void Serialize(FArchive& Ar) override;
#endif

protected:

	UFUNCTION()
	void OnRep_Health();

protected:

	//UPROPERTY(EditDefaultsOnly, Category = FX)
	//UParticleSystem* HitFX;

	UPROPERTY(SaveGame, Replicated, BlueprintReadWrite, VisibleAnywhere, Category = Movement)
	bool bIsRunning;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Movement)
	float RunningSpeedModifier;

	UPROPERTY(Replicated, BlueprintReadWrite, VisibleAnywhere, Category = Team)
	FGenericTeamId TeamId;

	UPROPERTY(SaveGame, BlueprintReadWrite, EditAnywhere, ReplicatedUsing=OnRep_Health, Category=Stats)
	float Health = 50;

	/** Low health ratio useful in some checks, [0 to 1] range */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Stats)
	float LowHealthRatio = 0.2f;

};
