// TopDownShooter project (c) 2016 V.Khmelevskiy, S.Zhernovoy

#include "TopDownShooter.h"
#include "UnrealNetwork.h"
#include "Player/TDItem.h"
#include "TDPlayerCharacter.h"
#include "TDPlayerController.h"
#include "Gameplay/TDInteractable.h"

#include "Perception/AIPerceptionSystem.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"

#include "TDGameMode.h"


static constexpr int32 MAX_WEAPON_NUM = 10;

// Sets default values
ATDPlayerCharacter::ATDPlayerCharacter(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicateMovement = true;

	PerceptionSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("PerceptionSource"));
	PerceptionSource->RegisterForSense(UAISense_Hearing::StaticClass());
	PerceptionSource->RegisterForSense(UAISense_Sight::StaticClass());
	PerceptionSource->RegisterWithPerceptionSystem();

	WeaponList.SetNumZeroed(MAX_WEAPON_NUM);

	// init default inventory
	Inventory.Items.SetNumZeroed((int32)EAmmoType::AMMO_TYPE_MAX);
	Inventory.Items[(int32)EAmmoType::Rockets] = FInventorySlot(UTDAmmoItem_Rockets::StaticClass(), 0);
	Inventory.Items[(int32)EAmmoType::Energy] = FInventorySlot(UTDAmmoItem_Energy::StaticClass(), 0);
	Inventory.Items[(int32)EAmmoType::Shells] = FInventorySlot(UTDAmmoItem_Shells::StaticClass(), 0);
	Inventory.Items[(int32)EAmmoType::Light] = FInventorySlot(UTDAmmoItem_Light::StaticClass(), 0);
	Inventory.Items[(int32)EAmmoType::Heavy] = FInventorySlot(UTDAmmoItem_Heavy::StaticClass(), 0);

	Inventory.Limits.SetNumZeroed((int32)EAmmoType::AMMO_TYPE_MAX);
	Inventory.Limits[(int32)EAmmoType::Rockets] = FInventorySlot(UTDAmmoItem_Rockets::StaticClass(), 12);
	Inventory.Limits[(int32)EAmmoType::Energy] = FInventorySlot(UTDAmmoItem_Energy::StaticClass(), 120);
	Inventory.Limits[(int32)EAmmoType::Shells] = FInventorySlot(UTDAmmoItem_Shells::StaticClass(), 90);
	Inventory.Limits[(int32)EAmmoType::Light] = FInventorySlot(UTDAmmoItem_Light::StaticClass(), 180);
	Inventory.Limits[(int32)EAmmoType::Heavy] = FInventorySlot(UTDAmmoItem_Heavy::StaticClass(), 40);

	TeamId = TeamIdPlayer;
}


// Called when the game starts or when spawned
void ATDPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();


	
}


// Called every frame
void ATDPlayerCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (IsDead())
		return;

	FRotator NewRot = (MouseHit - GetActorLocation()).Rotation();
	NewRot.Roll = 0;
	NewRot.Pitch = 0;
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), NewRot, DeltaTime, 300));
}


void ATDPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	if (Role < ROLE_Authority)
		return;

	if (DefaultWeapons.Num())
	{
		for (TSubclassOf<ATDWeaponBase> WeaponClass : DefaultWeapons)
		{
			ATDWeaponBase* SpawnedWeapon =  GetWorld()->SpawnActor<ATDWeaponBase>(WeaponClass);
			SpawnedWeapon->AttachToActor(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
			SpawnedWeapon->SetWeaponOwner(this);
			SpawnedWeapon->SetActorHiddenInGame(true);
			
			// TODO: fix hack. serialized weapon got GC'ed for some reason
			if (auto* SerializedWeapon = WeaponList[SpawnedWeapon->WeaponSlotIndex])
			{
				SpawnedWeapon->ClipAmmo = SerializedWeapon->ClipAmmo;
				SpawnedWeapon->ClipAmmoSecondary = SerializedWeapon->ClipAmmoSecondary;
			}
			WeaponList[SpawnedWeapon->WeaponSlotIndex] = SpawnedWeapon;
		}
	}
	// TODO: after spawning default inventory we should also remove/create weapons from serialization

	if (CurrentWeapon)
		OnEquipWeapon(CurrentWeapon->WeaponSlotIndex);
}


// Called to bind functionality to input
void ATDPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAction("WeaponSlot1", IE_Pressed, this, &ATDPlayerCharacter::EquipWeapon<1>);
	InputComponent->BindAction("WeaponSlot2", IE_Pressed, this, &ATDPlayerCharacter::EquipWeapon<2>);
	InputComponent->BindAction("WeaponSlot3", IE_Pressed, this, &ATDPlayerCharacter::EquipWeapon<3>);
	InputComponent->BindAction("WeaponSlot4", IE_Pressed, this, &ATDPlayerCharacter::EquipWeapon<4>);
	InputComponent->BindAction("WeaponSlot5", IE_Pressed, this, &ATDPlayerCharacter::EquipWeapon<5>);
	InputComponent->BindAction("WeaponSlot6", IE_Pressed, this, &ATDPlayerCharacter::EquipWeapon<6>);
	InputComponent->BindAction("WeaponSlot7", IE_Pressed, this, &ATDPlayerCharacter::EquipWeapon<7>);
	InputComponent->BindAction("WeaponSlot8", IE_Pressed, this, &ATDPlayerCharacter::EquipWeapon<8>);
	InputComponent->BindAction("WeaponSlot9", IE_Pressed, this, &ATDPlayerCharacter::EquipWeapon<9>);
	InputComponent->BindAction("WeaponSlot0", IE_Pressed, this, &ATDPlayerCharacter::EquipWeapon<0>);

	InputComponent->BindAction("Reload", IE_Pressed, this, &ATDPlayerCharacter::ReloadWeapon);

	InputComponent->BindAction("Activate", IE_Pressed, this, &ATDPlayerCharacter::Interact);

	InputComponent->BindAxis("CameraZoom", this, &ATDPlayerCharacter::AddCameraHeight);

}


void ATDPlayerCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone   - ( макрос DOREPLIFETIME - реплицировать "всем")
	DOREPLIFETIME(ATDPlayerCharacter, MouseHit);

	DOREPLIFETIME(ATDPlayerCharacter, CurrentWeapon);
	DOREPLIFETIME(ATDPlayerCharacter, Armor);
	DOREPLIFETIME(ATDPlayerCharacter, bIsPrimaryAttack);
	DOREPLIFETIME(ATDPlayerCharacter, bIsSecondaryAttack);

	// replicate to others
	//DOREPLIFETIME_CONDITION(ATopDawnBaseHero, bIsPrimaryAttack, COND_SkipOwner);
	//DOREPLIFETIME_CONDITION(ATopDawnBaseHero, bIsSecondaryAttack, COND_SkipOwner);

	// replicate only to self
	DOREPLIFETIME_CONDITION(ATDPlayerCharacter, WeaponList,          COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATDPlayerCharacter, Inventory,		   COND_OwnerOnly);
}


void ATDPlayerCharacter::SetIsPrimaryAttack(bool NewValue)
{
	bIsPrimaryAttack = NewValue;

	if (CurrentWeapon)
	{
		bIsPrimaryAttack ? CurrentWeapon->StartFirePrimary() : CurrentWeapon->StopFirePrimary();
	}
}


void ATDPlayerCharacter::SetIsSecondaryAttack(bool NewValue)
{
	bIsSecondaryAttack = NewValue;
}


void ATDPlayerCharacter::SetMouseHit(FVector NewMouseHit)
{
	MouseHit = NewMouseHit;

	if(!HasAuthority())
	{
		ServerSetMouseHit(NewMouseHit);
	}
}

bool ATDPlayerCharacter::ServerSetMouseHit_Validate(FVector NewMouseHit)
{
	return true;
}


void ATDPlayerCharacter::ServerSetMouseHit_Implementation(FVector NewMouseHit)
{
	MouseHit = NewMouseHit;
}


// TODO: remove this method
FVector ATDPlayerCharacter::GetWeaponMuzzleSocket(FName SocketName)
{
	return FVector::ZeroVector;
}


FVector ATDPlayerCharacter::GetWeaponAim() const
{
	return MouseHit + FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
}


void ATDPlayerCharacter::EquipWeapon(int32 WeaponIndex)
{
	if (IsDead())
		return;

	OnEquipWeapon(WeaponIndex);

	if (!HasAuthority())
	{
		ServerEquipWeapon(WeaponIndex);
	}
}


void ATDPlayerCharacter::OnEquipWeapon(int32 WeaponIndex)
{
	if (CurrentWeapon && CurrentWeapon->WeaponSlotIndex == WeaponIndex)
	{
		return;
	}

	if (ATDWeaponBase* Weapon = WeaponList[WeaponIndex])
	{
		if (CurrentWeapon)
		{
			CurrentWeapon->UnEquipWeapon();
		}

		Weapon->SetWeaponOwner(this);
		//Weapon->SetOwner(this);

		Weapon->EquipWeapon();
		CurrentWeapon = Weapon;
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, HandSocket);

		if (bIsPrimaryAttack)
		{
			CurrentWeapon->StartFirePrimary();
		}

		if (bIsSecondaryAttack)
		{
			CurrentWeapon->StartFireSecondary();
		}
	}
}


void ATDPlayerCharacter::ReloadWeapon()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartReload();
   }

	if (!HasAuthority())
	{
		ServerReloadWeapon();
	}
}


bool ATDPlayerCharacter::CanPickUpItem(TSubclassOf<UTDItem> Item)
{
	FInventorySlot Slot;
	FInventorySlot Limit;
	if ( -1 != FindItemSlot(Item, Slot, Limit))
	{
		if (Slot.Count >= Limit.Count)
		{
			return false;
		}
	}

	return true;
}


int32 ATDPlayerCharacter::FindItemSlot(TSubclassOf<UTDItem> ItemType, FInventorySlot& Item, FInventorySlot& Limit)
{
	bool found = false;
	bool foundlimit = false;
	int32 index = -1;

	if (!ItemType)
	{
		return -1;
	}

	for (FInventorySlot& Slot : Inventory.Items)
	{
		index++;
		if (Slot.Item == ItemType)
		{
			found = true;

			Item = Slot;
			break;
		}
	}

	for (FInventorySlot& Slot : Inventory.Limits)
	{
		if (Slot.Item == ItemType)
		{
			Limit = Slot;
			foundlimit = true;
			break;
		}
	}

	return found ? index : -1;
}


void ATDPlayerCharacter::InventoryAddItem(TSubclassOf<UTDItem> Item, int32 ItemCount)
{
	FInventorySlot Slot;
	FInventorySlot Limit;

	if (!Item)
	{
		return;
	}

	int32 index = FindItemSlot(Item, Slot, Limit);
	
	if( index  == -1)
	{
		index = Inventory.Items.Add(FInventorySlot(Item, 0));
	} 

	Inventory.Items[index].Count += ItemCount;

	if (Limit.Count)
	{
		Inventory.Items[index].Count = FMath::Clamp<int32>(Inventory.Items[index].Count, 0, Limit.Count);
	}
}


int32 ATDPlayerCharacter::InventoryGetAmmoCount(EAmmoType AmmoType)
{
	int32 result = Inventory.Items[(int32)AmmoType].Count;
	return result;
}


int32 ATDPlayerCharacter::InventoryGetMaxAmmoCount(EAmmoType AmmoType)
{
	int32 result = Inventory.Limits[(int32)AmmoType].Count;
	return result;
}


bool ATDPlayerCharacter::IsFullArmor()
{
	return Armor >= GetMaxArmor();
}


void ATDPlayerCharacter::AddArmor(float ArmorToAdd)
{
	Armor += ArmorToAdd;
	Armor = FMath::Clamp<float>(Armor, 0, GetMaxArmor());
}


bool ATDPlayerCharacter::ServerReloadWeapon_Validate()
{
	return true;
}


void ATDPlayerCharacter::ServerReloadWeapon_Implementation()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartReload();
	}
}


bool ATDPlayerCharacter::ServerEquipWeapon_Validate(int32 WeaponIndex)
{
	return true;
}


void ATDPlayerCharacter::ServerEquipWeapon_Implementation(int32 WeaponIndex)
{
	OnEquipWeapon(WeaponIndex);
}


float ATDPlayerCharacter::GetMaxArmor() const
{
	return MaxArmor;
}


int32 ATDPlayerCharacter::InventoryHasAmmo(EAmmoType AmmoType, int32 AmmoCount)
{
	int32 Ammount = InventoryGetAmmoCount(AmmoType);
	Ammount = FMath::Clamp<float>(Ammount, 0, AmmoCount);
	return Ammount;
}


void ATDPlayerCharacter::InventoryRemoveAmmo(EAmmoType AmmoType, int32 AmmoCount)
{
	int32 AmmoToRemove = InventoryHasAmmo(AmmoType, AmmoCount);
	Inventory.Items[(int32)AmmoType].Count -= AmmoToRemove;
}


float ATDPlayerCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float damage = Damage;

	if (Armor > 0)
	{
		if (Damage > Armor)
		{
			float damageLeft = damage - Armor;
			damage = Armor;
			Armor = 0;
			damage += Super::TakeDamage(damageLeft, DamageEvent, EventInstigator, DamageCauser);
		}
		else // damage <= armor
		{
			Armor -= Damage;
		}
	}
	else // no armor
	{
		damage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	}

	return damage;
}


void ATDPlayerCharacter::Die()
{
	Super::Die();

	SetLifeSpan(0);

	GetCharacterMovement()->SetJumpAllowed(false);

#if 0
	ATDPlayerController* PlayerController = Cast<ATDPlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->SetCinematicMode(true, true, true);
	}
#endif

	if (HasAuthority())
	{
		ATDGameMode* GameMode = Cast<ATDGameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{
			GameMode->OnPlayerKilled();
		}
	}

}


void ATDPlayerCharacter::SetCameraVolumeHiding(UPrimitiveComponent* Component) const
{
	ATDPlayerController* PC = Cast<ATDPlayerController>(Controller);
	if (PC)
	{
		PC->SetupCameraVolumeBlocking(Component);
	}
}


bool ATDPlayerCharacter::IsLowAmmo(EAmmoType AmmoType) const
{
	int32 InventoryAmmo = Inventory.Items[(int32)AmmoType].Count;
	int32 LimitAmmo = Inventory.Limits[(int32)AmmoType].Count;

	// Ammo/MaxAmmo rate
	float ammoRate = InventoryAmmo / LimitAmmo;

	return ammoRate < LowAmmoRatio; 
}


bool ATDPlayerCharacter::IsLowArmor() const
{
	return (Armor / GetMaxArmor()) < LowArmorRatio;
}


// TODO: it would be more useful to return collection instead of single value
EAmmoType ATDPlayerCharacter::GetLowestAmmoType() const
{
	TArray<EAmmoType> HasAmmoTypes;

	// get all used ammo types
	for (const ATDWeaponBase* weapon : WeaponList )
	{
		if( !weapon )
			continue;

		HasAmmoTypes.AddUnique(weapon->WeaponConfig.AmmoType);
	}
	
	float lowestRatio=1;
	int32 lowestType = (int32)EAmmoType::AMMO_TYPE_MAX;
	for (int ammoType = 0; ammoType < ((int32)EAmmoType::AMMO_TYPE_MAX) - 1; ammoType++)
	{
		float ratio = Inventory.Items[ammoType].Count / Inventory.Limits[ammoType].Count;
		
		// only interested if ammo low
		if ( ratio > LowAmmoRatio)
			continue;

		// slight chance to drop when ammo really low for other types
		if ( !HasAmmoTypes.Contains((EAmmoType)lowestType) && ratio < 0.1f)
			continue;

		if (ratio < lowestRatio )
		{
			lowestRatio = ratio;
			lowestType = ammoType;
		}
	}

	return lowestType < (int32)EAmmoType::AMMO_TYPE_MAX ? (EAmmoType)lowestType : EAmmoType::AMMO_TYPE_MAX;
}


void ATDPlayerCharacter::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	if (Ar.IsSaveGame())
	{
		// serialize armor
		Ar << Armor;

		// serialize ammo
		for (int32 i = 0; i < (int32)EAmmoType::AMMO_TYPE_MAX; i++)
		{
			Ar << Inventory.Items[i].Count;
		}


		// serialize weapons and clip ammo
		for (int32 i = 0; i < MAX_WEAPON_NUM; i++)
		{
			TSubclassOf<ATDWeaponBase> WeaponClass = nullptr;
			int32 ClipAmmo = 0;
			int32 ClipAmmoSecondary = 0;

			// first fetch actual values if saving
			if (Ar.ArIsSaving)
			{
				if (WeaponList[i])
				{
					WeaponClass = WeaponList[i]->GetClass();
					ClipAmmo = WeaponList[i]->ClipAmmo;
					ClipAmmoSecondary = WeaponList[i]->ClipAmmoSecondary;
				}
			}

			Ar << WeaponClass;
			Ar << ClipAmmo;
			Ar << ClipAmmoSecondary;

			// when loading first destroy existing weapons, then spawn what stored and apply ammo count
			if (Ar.ArIsLoading)
			{
				CurrentWeapon = nullptr;

				for (auto* Weapon : WeaponList)
				{
					if (Weapon)
					{
						Weapon->SetLifeSpan(0.1f);
						Weapon->SetWeaponOwner(nullptr);
					}
				}

				if (WeaponClass)
				{

					ATDWeaponBase* SpawnedWeapon = GetWorld()->SpawnActor<ATDWeaponBase>(WeaponClass);
					SpawnedWeapon->AttachToActor(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
					SpawnedWeapon->SetWeaponOwner(this);
					SpawnedWeapon->SetActorHiddenInGame(true);
					SpawnedWeapon->ClipAmmo = ClipAmmo;
					SpawnedWeapon->ClipAmmoSecondary = ClipAmmoSecondary;

					WeaponList[SpawnedWeapon->WeaponSlotIndex] = SpawnedWeapon;
				}
				else
				{
					WeaponList[i] = nullptr;
				}
			} // if (Ar.ArIsLoading)

		} // for (int32 i = 0; i < MAX_WEAPON_NUM; i++)

		// serialize selected weapon index
		{
			int32 selectedWeapon = 0;
			if (CurrentWeapon)
			{
				selectedWeapon = CurrentWeapon->WeaponSlotIndex;
			}

			Ar << selectedWeapon;

			if (Ar.ArIsLoading)
			{
				EquipWeapon(selectedWeapon);
			}
		}
	}
}


void ATDPlayerCharacter::OnRep_PrimaryAttack()
{
	if (CurrentWeapon)
	{
		bIsPrimaryAttack ? CurrentWeapon->StartFirePrimary() : CurrentWeapon->StopFirePrimary();
	}
}


void ATDPlayerCharacter::OnRep_SecondaryAttack()
{

}


void ATDPlayerCharacter::Interact()
{
	Interact_Impl();

	if (!HasAuthority())
		Server_Interact();
}


bool ATDPlayerCharacter::Server_Interact_Validate()
{
	return true;
}


void ATDPlayerCharacter::Server_Interact_Implementation()
{
	Interact_Impl();
}


void ATDPlayerCharacter::Interact_Impl()
{
	if (IsDead())
		return;

	TArray<AActor*> Interactables;
	GetOverlappingActors(Interactables, UTDInteractable::StaticClass());

	for (AActor* Actor : Interactables)
	{
		// FIX ME: wtf? engine bug? filter not working, we get all overlaps that leads to crash when attempt to call interface
		if (Actor->Implements<UTDInteractable>())
		{
			ITDInteractable::Execute_OnInteract(Actor, this);
		}
	}
}
