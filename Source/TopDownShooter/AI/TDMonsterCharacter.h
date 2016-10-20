// TopDownShooter project (c) 2016 V.Khmelevskiy

#pragma once

#include "Player/TDCharacter.h"
#include "Runtime/AIModule/Classes/Perception/AIPerceptionTypes.h"
#include "TDSaveGame.h"
#include "TDMonsterCharacter.generated.h"

class ATDAIController;
class UBehaviorTree;
class UAIPerceptionComponent;
class UAISenseConfig_Hearing;
class UAISenseConfig_Sight;

class ATDPickUp;
class ATDPlayerCharacter;


//UCLASS()
class FLootDropData //: public UObject
{
	//GENERATED_BODY()

public:
	// Init with chance of drop like (ChanceRange.X <= Chance <= ChanceRange.Y)
	FLootDropData(TSubclassOf<ATDPickUp> ItemClass, FVector2D ChanceRange);

	FLootDropData(TSubclassOf<ATDPickUp> ItemClass, FVector2D ChanceRange, TFunction<bool(ATDPlayerCharacter*)> DropConditionDelegate);

	void operator()(ATDPlayerCharacter* Owner, UWorld* WorldContext, const FTransform& Transform);

protected:
	//UFUNCTION()
	//void RollDrop(TFunctionRef<bool()> DropDelegate);

	//UPROPERTY()
	TSubclassOf<ATDPickUp> Item;

	//UPROPERTY()
	FVector2D Chance;

	// drop condition
	TFunction<bool(ATDPlayerCharacter*)> DropCondition;

private:
	FLootDropData() {}
};

USTRUCT(Blueprintable)
struct FAttackMode
{
	GENERATED_BODY()

	/// Attack mode name used for log/display/comparing purposes.
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = Attack)
	FName Name;

	/// Minimum range from which attack can be performed.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Attack)
	float MinRange;

	/// Maximum range from which attack can be performed. 0 = no limit
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Attack)
	float MaxRange;

	/// Cooldown time (seconds) before attack could be performed again. 0 = no delay
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Attack)
	float Cooldown;

	/// Attack damage
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Attack)
	float Damage;
	
	/// Projectile template. If set indicates this is ranged mode attack
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Attack)
	TSubclassOf<ATDProjectileBase> ProjectileClass;


	bool operator==(const FAttackMode &ls) const
	{
		return Name == ls.Name;
	}
};

/**
 * 
 */
UCLASS()
class TOPDOWNSHOOTER_API ATDMonsterCharacter : public ATDCharacter, public ITDSaveableInterface
{
	GENERATED_BODY()

public :

	ATDMonsterCharacter(const FObjectInitializer& ObjectInitializer);

	void InitDropData();

	virtual void PossessedBy(AController* NewController) override;
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	
	virtual void Die() override;

	UFUNCTION(BlueprintCallable, category = Attack)
	void SetIsAttacking(bool bAttacking);

	UFUNCTION(BlueprintCallable, category = Attack)
	bool GetIsAttacking() const;

	/* Event called when animation reaches hit position 
	* @param AttackModeName Which mode used for this attack (optional)
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Attack)
	void OnAttackHit(FName AttackModeName = NAME_None);

protected:
	//////////////////////////////////////////////////////////////////////////
	// UAIPerceptionComponent Delegates

	UFUNCTION()
	void OnUpdatePerception(TArray<AActor*> UpdatedActors);

	UFUNCTION()
	void OnActorPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);

	///////////////////////////

	void ForceUpdatePerception();

	UFUNCTION(BlueprintCallable, Category = Attack)
	void DamageActorsInVolume(UBoxComponent* VolumeActor, FName AttackModeName);

	void DropLoot() const;

	// helper function
	void DropItem(TSubclassOf<ATDPickUp> ItemPickup, ATDPlayerCharacter* Player) const;

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, category = AI)
	UBehaviorTree* BehaviorTree;

	UPROPERTY( BlueprintReadOnly, VisibleAnywhere, category = Perception)
	UAIPerceptionComponent* PerceptionComponent;

	UPROPERTY(transient)
	UAISenseConfig_Hearing* HearingConfig;

	UPROPERTY(transient)
	UAISenseConfig_Sight* SightConfig;



	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = Attack)
	int32 HasAttackMode(const FAttackMode& AttackMode) const;

	UFUNCTION(BlueprintCallable, Category = Attack)
	int32 HasAttackModeName(FName AttackName) const;

	UFUNCTION(BlueprintPure, Category=Attack)
	bool CanAttackEnemyWithMode(const AActor* const Enemy, const FAttackMode& AMode, int32& OutModeIndex) const;

	// Variant for checking if attack possibly taking mode by name
	UFUNCTION(BlueprintPure, Category = Attack)
	bool CanAttackEnemyWithModeName(const AActor* const Enemy, FName ModeName, int32& OutModeIndex) const;

	const TArray<FAttackMode>& GetAttackModes() const { return AttackModes; }

	// (WIP) Perform attack(triggers damage overlap events, play particles, etc.) and set cooldown
	UFUNCTION(BlueprintCallable, Category = Attack)
	void AttackEnemyWithMode(const AActor* const Enemy, const FAttackMode& AttackMode);

	UFUNCTION(BlueprintCallable, Category = Attack)
	void AttackEnemyWithModeName(const AActor* const Enemy, FName ModeName);

	UFUNCTION(BlueprintCallable, Category = Attack)
	void SetAttackModeCooldown(const FAttackMode& AttackMode);

	UFUNCTION(BlueprintCallable, Category = Attack)
	void SetAttackModeNameCooldown(FName AttackName);

	// Helper method, sets attack mode which used to play animation
	UFUNCTION()
	void SetCurrentAttackMode(FName NewAttackMode);



	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// virtual void WriteActorData(FArchive& Ar) override;

protected:
	UPROPERTY()
	ATDAIController* AICon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Attack)
	TArray<FAttackMode> AttackModes;

	// Stores last attack time for each mode
	UPROPERTY()
	TArray<float> AttackModeLastTime;

	UPROPERTY(replicated, BlueprintReadWrite, VisibleAnywhere, category = Attack)
	bool bIsAttacking;

	// Attack mode name currently playing (used for animation)
	UPROPERTY(replicated, BlueprintReadWrite, VisibleAnywhere, category = Attack)
	FName CurrentAttackMode;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, category = Attack)
	FName RangedSocketName;

	FTimerHandle PercUpdateTimer;
public:
	static void ResetDropData();
};
