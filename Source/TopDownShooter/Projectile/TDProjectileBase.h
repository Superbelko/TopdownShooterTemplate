// TopDownShooter project (c) 2016 V.Khmelevskiy

#pragma once

#include "GameFramework/Actor.h"
#include "Weapon/TDWeaponBase.h"
#include "TDProjectileBase.generated.h"

class ATDPlayerCharacter;
class ATDCharacter;

UCLASS()
class TOPDOWNSHOOTER_API ATDProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	

	ATDProjectileBase(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	
	virtual void Tick( float DeltaSeconds ) override;

	virtual void PostInitializeComponents() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void HitActor(const FHitResult& ImpactResult); 

	void SetProjectileConfig(FWeaponConfig NewWeaponConfig, const ATDCharacter* NewCharacter);

	UFUNCTION(BlueprintPure, Category=Projectile)
	FVector GetProjectileVelocity() const;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UFUNCTION()
	void SetWeapon(ATDWeaponBase* OwnerWeapon);

	UFUNCTION()
	void OnRep_WeaponOwner();

protected:

	UPROPERTY(ReplicatedUsing = OnRep_WeaponOwner)
	ATDWeaponBase* WeaponOwner;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, category = Projectile)
	bool bDoOverlapDamage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, category = Projectile)
	float HitForce;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, category = Projectile)
	bool bHideOnHit;

	FWeaponConfig WeaponConfig;

	TWeakObjectPtr<ATDCharacter> Character;

	TWeakObjectPtr<ATDWeaponBase> Weapon;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, category = Config)
	UParticleSystemComponent* ProjectileFX;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, category = Config)
	USphereComponent* Sphere;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, category = Config)
	UProjectileMovementComponent* ProjectileMC;
};
