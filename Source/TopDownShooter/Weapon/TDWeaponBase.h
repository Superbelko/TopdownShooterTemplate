// TopDownShooter project (c) 2016 V.Khmelevskiy, S.Zhernovoy

#pragma once

#include "GameFramework/Actor.h"
#include "TDWeaponBase.generated.h"

class ATDPlayerCharacter;
class ATDProjectileBase;
class ATDWeaponBase;
class ATDImpactEffect;

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	Light,
	Shells,
	Energy,
	Rockets,
	Heavy,
	AMMO_TYPE_MAX,
};

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Idle,
	Reload,
	Fire,
};

USTRUCT(BlueprintType)
struct FWeaponConfig
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, category = Config)
	float Damage = 1.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, category = Config)
	float FireRate = 1.0f;

	/** Bullet count in one shot, used in shotgun-like weapons */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, category = Config)
	int32 BulletsPerShot = 1;

	// reload time when no animation is set
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, category = Config)
	float NoAnimReloadTime = 1.0f;

	/** Weapon maximum accuracy falloff angle in degrees */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, category = Config)
	float AccuracyCone;

	// TODO: should remove this?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, category = Config)
	int32 MaxClipAmmo;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, category = Config)
	EAmmoType AmmoType;
};


UCLASS(Abstract)
class TOPDOWNSHOOTER_API ATDWeaponBase : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ATDWeaponBase(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	UFUNCTION(BlueprintCallable, category = Weapon)
	void StartFirePrimary();

	UFUNCTION(BlueprintCallable, category = Weapon)
	void StartFireSecondary();

	UFUNCTION(unreliable, server, WithValidation)
	void ServerStartFirePrimary();

	UFUNCTION(unreliable, server, WithValidation)
	void SeverStartFireSecondary();

	UFUNCTION(BlueprintCallable, category = Weapon)
	void StopFirePrimary();

	UFUNCTION(BlueprintCallable, category = Weapon)
	void StopFireSecondary();

	UFUNCTION(unreliable, server, WithValidation)
	void ServerStopFirePrimary();

	UFUNCTION(unreliable, server, WithValidation)
	void ServerStopFireSecondary();


	UFUNCTION(BlueprintCallable, category = Weapon)
	void StartReload();

	UFUNCTION(unreliable, server, WithValidation)
	void ServerStartReload();

	UFUNCTION(BlueprintCallable, category = Weapon)
	void EquipWeapon();

	UFUNCTION(unreliable, server, WithValidation)
	void ServerEquipWeapon();

	UFUNCTION(BlueprintCallable, category = Weapon)
	void UnEquipWeapon();

	UFUNCTION(unreliable, server, WithValidation)
	void ServerUnEquipWeapon();

	UFUNCTION(unreliable, NetMulticast)
	void MulticastUnEquipWeapon();

	UFUNCTION(BlueprintCallable, category = Weapon)
	void SetWeaponOwner(ATDPlayerCharacter* NewHero);

	UFUNCTION(reliable, server, WithValidation)
	void ServerSetWeaponOwner(ATDPlayerCharacter* NewHero);

protected:

	UPROPERTY(Replicated)
	ATDPlayerCharacter* WeaponOwner;

	void UpdateWeaponState();

	UFUNCTION(unreliable, client)
	void ClientUpdateAmmo(int32 Primary, int32 Secondary);

	UFUNCTION()
	void OnRep_SecondaryMode();

	UFUNCTION()
	void OnRep_WantFire();

	UFUNCTION()
	void OnRep_WantFireSecondary();

	UFUNCTION()
	void OnRep_Accuracy();

	int32 PlayerHasAmmo(int32 Ammo, bool bSecondary = false);

	void FirePrimary();
	void FireSecondary();

	void BeginFirePrimary();
	void EndFirePrimary();

	void BeginReload();
	void StopReload();
	void FinishReload();

	void ConsumeAmmo(int32 AmmoCount = 1, bool bSecondaryAmmo = false);

	/** play weapon sounds */
	UAudioComponent* PlayWeaponSound(USoundCue* Sound);

public:

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, category = Mesh)
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, category = Config)
	USphereComponent* Sphere;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Effects)
	TSubclassOf<ATDImpactEffect> ImpactEffects;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, category = UI)
	UTexture2D* WeaponIcon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Config)
	FWeaponConfig WeaponConfig;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Config)
	FWeaponConfig WeaponConfigSecondary;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, category = Weapon)
	int32 WeaponSlotIndex;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, category = Weapon)
	bool bInfiniteClip;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, category = Weapon)
	bool bInfiniteAmmo;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, category = Weapon)
	int32 ClipAmmo;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, category = Weapon)
	int32 ClipAmmoSecondary;

	UPROPERTY(ReplicatedUsing = OnRep_Accuracy, BlueprintReadWrite, EditAnywhere, category = Weapon)
	int32 AccuracyFalloffSeed;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, category = config)
	TSubclassOf<ATDProjectileBase> ProjectileClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, category = config)
	TSubclassOf<ATDProjectileBase> ProjectileSecondaryClass;

	UPROPERTY(ReplicatedUsing = OnRep_SecondaryMode, BlueprintReadWrite, EditAnywhere, category = Config)
	bool bHasSecondaryAttack;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, category = Config)
	FName MuzzleSocket = FName(TEXT("Muzzle"));

	/** firing audio (bLoopedFireSound set) */
	UPROPERTY(Transient)
	UAudioComponent* FireAC;

	/** FX for muzzle flash */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UParticleSystem* MuzzleFX;

	/** spawned component for muzzle FX */
	UPROPERTY(Transient)
	UParticleSystemComponent* MuzzlePSC;

	/** single fire sound (bLoopedFireSound not set) */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* FireSound;

	/** looped fire sound (bLoopedFireSound set) */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* FireLoopSound;

	/** finished burst sound (bLoopedFireSound set) */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* FireFinishSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	bool bLoopedFireSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	bool bLoopedMuzzleFX;

	float LastShotTime;

	UPROPERTY(Transient, ReplicatedUsing=OnRep_WantFire)
	bool bWantFire;

	UPROPERTY(Transient, ReplicatedUsing=OnRep_WantFireSecondary)
	bool bWantFireSecondary;

	bool bWantReload;
	bool bWantUnEquip;

	// used for replication reload animation
	//UPROPERTY(Transient, ReplicatedUsing=OnRep_Reload)
	//bool bPendingReload;

	// if true cancels primary attack mode when shoot secondary
	bool bPreferSecondary;

	// is both primary and secondary fire allowed at same time
	bool bAllowCombinedFiring;

	
	bool bIsFire;
	bool bIsFireSecondary;

protected:

	EWeaponState WeaponState;
	
	FTimerHandle RefireTimer;
	FTimerHandle RefireSecondaryTimer;

	FTimerHandle ReloadTimer;

	// weapon spread stream, synchronized
	FRandomStream AccuracyRand;
};
