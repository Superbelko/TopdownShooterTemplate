// TopDownShooter project (c) 2016 V.Khmelevskiy, S.Zhernovoy

#include "TopDownShooter.h"

#include "UnrealNetwork.h"

#include "TDWeaponBase.h"
#include "Player/TDPlayerCharacter.h"
#include "Projectile/TDProjectileBase.h"



// Sets default values
ATDWeaponBase::ATDWeaponBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

	// add   Sphere
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));

	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Sphere->SetCollisionObjectType(COLLISION_PICKUP);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// set our root
	SetRootComponent(Sphere);

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("MeshComp");
	WeaponMesh->SetupAttachment(Sphere);
	//WeaponMesh->SetVisibility(false);

}

// Called when the game starts or when spawned
void ATDWeaponBase::BeginPlay()
{
	Super::BeginPlay();

}

void ATDWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATDWeaponBase, WeaponOwner);
	DOREPLIFETIME(ATDWeaponBase, AccuracyFalloffSeed);
	
 	DOREPLIFETIME_CONDITION(ATDWeaponBase, ClipAmmo,              COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATDWeaponBase, ClipAmmoSecondary,     COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATDWeaponBase, bHasSecondaryAttack,   COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(ATDWeaponBase, bWantFire,             COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ATDWeaponBase, bWantFireSecondary,    COND_SkipOwner);
	//DOREPLIFETIME_CONDITION(ATDWeaponBase, bPendingReload, COND_SkipOwner);
}

void ATDWeaponBase::StartFirePrimary()
{
	bWantFire = true;
	UpdateWeaponState();

	if (!HasAuthority())
	{
		ServerStartFirePrimary();
	}
}

bool ATDWeaponBase::ServerStartFirePrimary_Validate()
{
	return true;
}

void ATDWeaponBase::ServerStartFirePrimary_Implementation()
{
	bWantFire = true;
	UpdateWeaponState();
}

void ATDWeaponBase::StartFireSecondary()
{
	bWantFireSecondary = true;
	UpdateWeaponState();

	if (!HasAuthority())
	{
		SeverStartFireSecondary();
	}

}

bool ATDWeaponBase::SeverStartFireSecondary_Validate()
{
	return true;
}

void ATDWeaponBase::SeverStartFireSecondary_Implementation()
{
	bWantFireSecondary = true;
	UpdateWeaponState();
}

void ATDWeaponBase::StopFirePrimary()
{
	bWantFire = false;
	UpdateWeaponState();

	if (!HasAuthority())
	{
		ServerStopFirePrimary();
	}

}

bool ATDWeaponBase::ServerStopFirePrimary_Validate()
{
	return true;
}

void ATDWeaponBase::ServerStopFirePrimary_Implementation()
{
	bWantFire = false;
	UpdateWeaponState();
}


void ATDWeaponBase::StopFireSecondary()
{
	bWantFireSecondary = false;
	UpdateWeaponState();

	if (!HasAuthority())
	{
		ServerStopFireSecondary();
	}
}


bool ATDWeaponBase::ServerStopFireSecondary_Validate()
{
	return true;
}

void ATDWeaponBase::ServerStopFireSecondary_Implementation()
{
	bWantFireSecondary = false;
	UpdateWeaponState();
}


void ATDWeaponBase::StartReload()
{
	if (bInfiniteClip)
		return;

	bWantReload = true;
	UpdateWeaponState();

	if (!HasAuthority())
	{
		ServerStartReload();
	}

}


bool ATDWeaponBase::ServerStartReload_Validate()
{
	return true;
}

void ATDWeaponBase::ServerStartReload_Implementation()
{
	if (bInfiniteClip)
		return;

	bWantReload = true;
	UpdateWeaponState();
}

//*********************************************** EQUIP AND UNEQUIP ********************************************
void ATDWeaponBase::EquipWeapon()
{
	SetActorHiddenInGame(false);

	if (!HasAuthority())
		ServerEquipWeapon();

	if (HasAuthority())
	{
		AccuracyFalloffSeed = FDateTime::Now().GetTicks() / 3001; // just magic number
	}
}

bool ATDWeaponBase::ServerEquipWeapon_Validate()
{
	return true;
}

void ATDWeaponBase::ServerEquipWeapon_Implementation()
{
	SetActorHiddenInGame(false);
}


void ATDWeaponBase::UnEquipWeapon()
{
	bWantUnEquip = true;
	SetActorHiddenInGame(true);

	UpdateWeaponState();

	if (!HasAuthority())
	{
		ServerUnEquipWeapon();
	}
}

bool ATDWeaponBase::ServerUnEquipWeapon_Validate()
{
	return true;
}

void ATDWeaponBase::ServerUnEquipWeapon_Implementation()
{
	SetActorHiddenInGame(true);
	bWantUnEquip = true;
	UpdateWeaponState();
}


void ATDWeaponBase::MulticastUnEquipWeapon_Implementation()
{
	UnEquipWeapon();
}


void ATDWeaponBase::SetWeaponOwner(ATDPlayerCharacter* NewHero)
{
	WeaponOwner = NewHero;
	SetOwner(NewHero);
	/*
	if (!HasAuthority())
	{
		ServerSetWeaponOwner(NewHero);
	}
	*/
}


bool ATDWeaponBase::ServerSetWeaponOwner_Validate(ATDPlayerCharacter* NewHero)
{
	return true;
}

void ATDWeaponBase::ServerSetWeaponOwner_Implementation(ATDPlayerCharacter* NewHero)
{
	// TODO: should we remove this function?
	WeaponOwner = NewHero;
	SetOwner(NewHero);
}

//**************************************************************************************************************

void ATDWeaponBase::UpdateWeaponState()
{
	// TODO: refactor to function (CanFireWeapon?)
	// stop firing if clip empty
	if (ClipAmmo < 1 && !bInfiniteClip)
	{
		//bWantFire = false;
		bWantReload = true;
		EndFirePrimary();
	}

	// stop firing if clip empty
	// 		if (ClipAmmoSecondary < 1)
	// 			EndFireSecondary();

	if (bWantUnEquip)
	{
		StopReload();
		EndFirePrimary();
		// EndFireSecondary();

		bWantFire = false;
		bWantFireSecondary = false;
		bWantReload = false;
		bWantUnEquip = false;

//		UnEquipWeapon();

		return;
	}

	if (bWantReload)
	{
		// already reloading
		if (ReloadTimer.IsValid())
		{
			return;
		}

		// don't reload if clip full
		if (GetClass()->GetDefaultObject<ATDWeaponBase>()->ClipAmmo == ClipAmmo)
		{
			return;
		}

		if (bInfiniteClip)
		{
			bWantReload = false;
			GetWorldTimerManager().ClearTimer(ReloadTimer);
			return;
		}

		// check if player has ammo
		if (!PlayerHasAmmo(1) && !PlayerHasAmmo(1, true))
		{
			return;
		}

		// dead can't reload
		if (WeaponOwner && WeaponOwner->IsDead())
		{
			bWantFire = false;
			bWantFireSecondary = false;
			return;
		}

		EndFirePrimary();

		BeginReload();
		return;
	}

	if (bWantFireSecondary)
	{
		if (!bAllowCombinedFiring && bPreferSecondary && RefireTimer.IsValid())
		{
			bWantFire = false;
			StopFirePrimary();
		}

		return;
	}

	if (bWantFire)
	{
		if (!ClipAmmo && !bInfiniteClip)
		{
			StartReload();
		}

		BeginFirePrimary();
		return;
	}


	WeaponState = EWeaponState::Idle;
	EndFirePrimary();
	/*StopReload();*/

	//EndFireSecondary();
}

void ATDWeaponBase::ClientUpdateAmmo_Implementation(int32 Primary, int32 Secondary)
{
	ClipAmmo = Primary;
	ClipAmmoSecondary = Secondary;
}

void ATDWeaponBase::OnRep_SecondaryMode()
{

}

void ATDWeaponBase::OnRep_WantFire()
{
	UpdateWeaponState();
}

void ATDWeaponBase::OnRep_WantFireSecondary()
{
	UpdateWeaponState();
}

void ATDWeaponBase::OnRep_Accuracy()
{
	AccuracyRand.Initialize(AccuracyFalloffSeed);
}

int32 ATDWeaponBase::PlayerHasAmmo(int32 Ammo, bool bSecondary /*= false*/)
{
	if (!WeaponOwner)
	{
		return 0;
	}			 
	return WeaponOwner->InventoryHasAmmo(WeaponConfig.AmmoType, Ammo);
}

//**************************************
// TODO: refactor function
void ATDWeaponBase::FirePrimary()
{
	constexpr float distance = 5000.0f;
	FVector MuzzleLoc;
	FVector FinalLocation;

	if (!WeaponOwner)
		return;

	if (!ClipAmmo && !bInfiniteClip)
	{
		bWantReload = true;
		UpdateWeaponState();
	}
		
	//MuzzleLoc =	WeaponOwner->GetWeaponMuzzleSocket(MuzzleSocket);
	MuzzleLoc = WeaponMesh->GetSocketLocation(MuzzleSocket);

	// get player aim and translate it forward to remove distance-height dependency
	FinalLocation = WeaponOwner->GetWeaponAim() - ( // actor position with adjusted height
		WeaponOwner->GetActorLocation()  
		+ FVector(0, 0, WeaponOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight())
		);
	FinalLocation.Normalize();
	FinalLocation = WeaponOwner->GetActorLocation() + FinalLocation * distance;

	FRotator MuzzleRot = (FinalLocation - MuzzleLoc).Rotation();

	// fire multiple bullets if we have to
	for (int32 bullet = 0; bullet < WeaponConfig.BulletsPerShot; bullet++)
	{
		// add accuracy falloff
		float aimOffset = AccuracyRand.FRandRange(-WeaponConfig.AccuracyCone, WeaponConfig.AccuracyCone);

		FTransform MuzzleTransform = FTransform(
			MuzzleRot.Add(aimOffset / 3, aimOffset, 0),
			MuzzleLoc);


		if (HasAuthority())
		{

			ATDProjectileBase* Projectile = GetWorld()->SpawnActor<ATDProjectileBase>(ProjectileClass->GetDefaultObject()->GetClass(), MuzzleTransform);
			if (Projectile)
			{
				Projectile->SetOwner(this);
				Projectile->SetWeapon(this);
				Projectile->SetProjectileConfig(WeaponConfig, WeaponOwner);
			}
		}
	}

	LastShotTime = GetWorld()->GetTimeSeconds();

	if (MuzzleFX)
	{
		//USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();

		// if looped fx spawn only if no particle system exists
		if (!bLoopedMuzzleFX || MuzzlePSC == nullptr)
		{
			MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, WeaponMesh, MuzzleSocket);
		}
	}

	if (bLoopedFireSound)
	{
		if (!FireAC)
		{
			FireAC = PlayWeaponSound(FireLoopSound);
		}
	}
	else
	{
		PlayWeaponSound(FireSound);
	}

	ConsumeAmmo();
	UpdateWeaponState();
}


void ATDWeaponBase::BeginFirePrimary()
{
	float NextShot = 0;

	NextShot = (LastShotTime + 1/WeaponConfig.FireRate) - GetWorld()->GetTimeSeconds();

	if (NextShot < 0)
		NextShot = 0;

	GetWorldTimerManager().SetTimer(RefireTimer, this, &ATDWeaponBase::FirePrimary, 1/WeaponConfig.FireRate, true, NextShot);
}

void ATDWeaponBase::EndFirePrimary()
{
	if (FireAC)
	{
		FireAC->FadeOut(0.1f, 0.0f);
		FireAC = nullptr;

		PlayWeaponSound(FireFinishSound);
	}

	if (bLoopedMuzzleFX)
	{
		if (MuzzlePSC)
		{
			MuzzlePSC->DeactivateSystem();
			MuzzlePSC = nullptr;
		}
	}

	GetWorldTimerManager().ClearTimer(RefireTimer);
}

void ATDWeaponBase::BeginReload()
{
	GetWorldTimerManager().SetTimer(ReloadTimer, this, &ATDWeaponBase::FinishReload, 1 , false);
}

void ATDWeaponBase::StopReload()
{
	GetWorldTimerManager().ClearTimer(ReloadTimer);
}

void ATDWeaponBase::FinishReload()
{
	if (!WeaponOwner)
	{
		return;
	}
	
	if (bInfiniteAmmo)
	{
		auto ClassDefaults = GetClass()->GetDefaultObject<ATDWeaponBase>();

		ClipAmmo = ClassDefaults->ClipAmmo;
		ClipAmmoSecondary = ClassDefaults->ClipAmmoSecondary;
	}
	else // bInfiniteAmmo = false
	{
		int32 HasAmmo = PlayerHasAmmo(GetClass()->GetDefaultObject<ATDWeaponBase>()->ClipAmmo - ClipAmmo);
		WeaponOwner->InventoryRemoveAmmo(WeaponConfig.AmmoType, HasAmmo);
		ClipAmmo += HasAmmo;
	}

	StopReload();
	bWantReload = false;

	UpdateWeaponState();
}


void ATDWeaponBase::ConsumeAmmo(int32 AmmoCount /*= 1*/, bool bSecondaryAmmo)
{
	if (bInfiniteClip)
		return;

	if (!bSecondaryAmmo)
	{
		ClipAmmo -= AmmoCount;
	}
	else
	{
		ClipAmmoSecondary -= AmmoCount;
	}
}


UAudioComponent* ATDWeaponBase::PlayWeaponSound(USoundCue* Sound)
{
	UAudioComponent* AC = nullptr;
	if (Sound && WeaponOwner)
	{
		AC = UGameplayStatics::SpawnSoundAttached(Sound, WeaponMesh);
	}

	return AC;
}