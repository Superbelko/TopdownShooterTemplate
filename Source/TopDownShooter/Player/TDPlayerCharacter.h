// TopDownShooter project (c) 2016 V.Khmelevskiy, S.Zhernovoy

#pragma once

#include "Player/TDCharacter.h"
#include "Weapon/TDWeaponBase.h"
#include "TDPlayerCharacter.generated.h"

class UTDItem;
class UAIPerceptionStimuliSourceComponent;

USTRUCT(Blueprintable)
struct FInventorySlot
{
	GENERATED_BODY()

	FInventorySlot() { Count = 0; }

	FInventorySlot(TSubclassOf<UTDItem> NewItem, int32 NewCount)
	{
		Item = NewItem;
		Count = NewCount;
	}

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, category = Inventory)
	TSubclassOf<UTDItem> Item;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = Inventory)
	int32 Count;
};

USTRUCT(Blueprintable)
struct FInventory
{
	GENERATED_BODY()

	/** Currently stored items with ammount */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, category = Inventory)
	TArray<FInventorySlot> Items;

	/** Item limits by type */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, category = Inventory)
	TArray<FInventorySlot> Limits;
};

UCLASS()
class TOPDOWNSHOOTER_API ATDPlayerCharacter : public ATDCharacter
{
	GENERATED_BODY()

public:
	ATDPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	
	virtual void Tick( float DeltaSeconds ) override;

	virtual void PostInitializeComponents() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	void SetIsPrimaryAttack(bool NewValue);

	void SetIsSecondaryAttack(bool NewValue);

	UFUNCTION(BlueprintCallable, Category = Mouse)
	void SetMouseHit(FVector NewMouseHit);

	UFUNCTION(Unreliable, Server, WithValidation, BlueprintCallable, Category = Mouse)
	void ServerSetMouseHit(FVector NewMouseHit);

	//USkeletalMeshComponent* GetWeaponMesh() { return WeaponMesh; }

	FVector GetWeaponMuzzleSocket(FName SocketName);

	FVector GetWeaponAim() const;

	template <int32 Index>
	void EquipWeapon() { EquipWeapon(Index); }

	UFUNCTION(BlueprintCallable, category = Weapon)
	void EquipWeapon(int32 WeaponIndex);

	void OnEquipWeapon(int32 WeaponIndex);

	UFUNCTION(unreliable, server, WithValidation, BlueprintCallable, category = Weapon)
	void ServerEquipWeapon(int32 WeaponIndex);

	UFUNCTION(BlueprintCallable, category = Weapon)
	void ReloadWeapon();

	UFUNCTION(unreliable, server, WithValidation, BlueprintCallable, category = Weapon)
	void ServerReloadWeapon();

	bool CanPickUpItem(TSubclassOf<UTDItem> Item);

	/** Get item slot or -1 if not found */
	int32 FindItemSlot(TSubclassOf<UTDItem> ItemType, FInventorySlot& Item , FInventorySlot& Limit);

	UFUNCTION(BlueprintCallable, category = Inventory)
	void InventoryAddItem(TSubclassOf<UTDItem> Item, int32 ItemCount);

	/** Get total ammo count */
	UFUNCTION(BlueprintCallable, category = Inventory)
	int32 InventoryGetAmmoCount(EAmmoType AmmoType);

	UFUNCTION(BlueprintCallable, category = Inventory)
	int32 InventoryGetMaxAmmoCount(EAmmoType AmmoType);

	bool IsFullArmor();

	void AddArmor(float ArmorToAdd);

	UFUNCTION(BlueprintCallable, Category=Stats)
	float GetMaxArmor() const;

	/** Get ammo count in range [0, InventoryAmmo] but not more than requested */
	int32 InventoryHasAmmo(EAmmoType AmmoType, int32 AmmoCount);

	/** Remove AmmoCount number from inventory or what remains */
	void InventoryRemoveAmmo(EAmmoType AmmoType, int32 AmmoCount);

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, category = Weapon)
	TArray<ATDWeaponBase*> WeaponList;

	/** Default player weapons given at level start */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = Weapon)
	TArray<TSubclassOf<ATDWeaponBase>> DefaultWeapons;


	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;


	virtual void Die() override;

	UFUNCTION(BlueprintCallable, Category = Camera)
	void SetCameraVolumeHiding(UPrimitiveComponent* Component) const;

	UFUNCTION(BlueprintImplementableEvent, Category=Camera)
	void AddCameraHeight(float Value);

	/** Returns true if ammo count is lower than LowAmmoRatio for ammo type */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Inventory)
	bool IsLowAmmo(EAmmoType AmmoType) const;

	/** Returns true if armor ratio lower than LowArmorRatio */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Inventory)
	bool IsLowArmor() const;

	/** Returns most needed ammo type, but only for owned weapon and if ammo lower than LowAmmoRatio */
	UFUNCTION(BlueprintCallable, Category=Inventory)
	EAmmoType GetLowestAmmoType() const;


	virtual void Serialize(FArchive& Ar) override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = Weapon)
	FName HandSocket = FName(TEXT("GunSocket"));

	UPROPERTY(replicated, VisibleAnywhere, BlueprintReadOnly, category = Weapon)
	ATDWeaponBase* CurrentWeapon;

	UPROPERTY(SaveGame, Replicated, BlueprintReadWrite, EditAnywhere, Category=Stats)
	float Armor;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Stats)
	float MaxArmor = 75;

	/** Low armor ratio useful in some checks, [0 to 1] range */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Stats)
	float LowArmorRatio = 0.2f;

	UPROPERTY(Replicated, BlueprintReadOnly, VisibleAnywhere, Category=Inventory)
	FInventory Inventory;

	/** Low ammo ratio */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Inventory)
	float LowAmmoRatio = 0.25f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = AI)
	UAIPerceptionStimuliSourceComponent* PerceptionSource;

	/** Min & Max range for camera spring arm */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Camera)
	FVector2D CameraHeightRange = FVector2D(400.0f,900.0f);

	UFUNCTION()
	void OnRep_PrimaryAttack();

	UFUNCTION()
	void OnRep_SecondaryAttack();

	/** Interact with buttons, triggers, etc. */
	UFUNCTION()
	void Interact();

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_Interact();

	void Interact_Impl();

private:

	UPROPERTY(ReplicatedUsing=OnRep_PrimaryAttack)
	bool bIsPrimaryAttack = false;

	UPROPERTY(ReplicatedUsing=OnRep_SecondaryAttack)
	bool bIsSecondaryAttack = false;

	UPROPERTY(Replicated)
	FVector MouseHit;

	
};
