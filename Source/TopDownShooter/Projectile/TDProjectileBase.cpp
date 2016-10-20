// TopDownShooter project (c) 2016 V.Khmelevskiy

#include "TopDownShooter.h"

#include "UnrealNetwork.h"

#include "TDProjectileBase.h"
#include "Effects/TDImpactEffect.h"
#include "Player/TDPlayerCharacter.h"


// Sets default values
ATDProjectileBase::ATDProjectileBase(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	// add   Sphere
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));

	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionObjectType(COLLISION_PROJECTILE);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	// set our root
	SetRootComponent(Sphere);

	// add   ProjectileMesh
	ProjectileFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ProjectileFX"));
	ProjectileFX->SetupAttachment(Sphere);
	
	// add Projectile movement component
	ProjectileMC = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMC"));

	InitialLifeSpan = 5;
}

// Called when the game starts or when spawned
void ATDProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATDProjectileBase::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ATDProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ProjectileMC->OnProjectileStop.AddDynamic(this, &ATDProjectileBase::HitActor);
}

void ATDProjectileBase::HitActor(const FHitResult& ImpactResult)
{
	FDamageEvent DamageEvent;
	DamageEvent.DamageTypeClass = UDamageType::StaticClass();
	AController* Controller = nullptr;
	ATDCharacter* MyChar = nullptr;

	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	if (bHideOnHit)
	{
		SetActorHiddenInGame(true);
	}

	SetLifeSpan(2.0f); // let particles fade
	ProjectileFX->DeactivateSystem();

	if (Character.IsValid())
	{
		MyChar = Character.Get();
		Controller = MyChar->GetController();
	}

	AActor* HitActor = ImpactResult.GetActor();
	if (HitActor && HitActor->IsValidLowLevel())
	{
		ATDCharacter* OtherChar = Cast<ATDCharacter>(HitActor);
		if (OtherChar && MyChar)
		{
			// don't damage friends
			if (OtherChar->GetGenericTeamId() == MyChar->GetGenericTeamId())
				return;
		}

		if (HasAuthority())
		{
			ImpactResult.GetActor()->TakeDamage(WeaponConfig.Damage, DamageEvent, Controller, this);
		}

		if (Weapon.IsValid())
		{
			ATDCharacter* WeaponOwner = Cast<ATDCharacter>(Weapon->GetOwner());
			if (WeaponOwner)
				WeaponOwner->SpawnImpactEffects(ImpactResult, Weapon->ImpactEffects);
		}
		
		// TODO: do we really need this?
		/*
		ImpactResult.GetActor()->ReceivePointDamage( WeaponConfig.Damage,
			UDamageType::StaticClass()->GetDefaultObject<UDamageType>(),
			ImpactResult.ImpactPoint,
			ImpactResult.ImpactNormal,
			ImpactResult.Component.IsValid() ? ImpactResult.Component.Get() : nullptr,
			ImpactResult.BoneName,
			GetActorRotation().GetInverse().Vector(),
			Controller,
			this);
		*/

		if (ImpactResult.GetActor()->IsRootComponentMovable())
		{
			UPrimitiveComponent* PC = Cast<UPrimitiveComponent>(ImpactResult.GetActor()->GetRootComponent());
			if ( IsValid(PC)
				&& PC->IsSimulatingPhysics() 
				&& HitForce != 0)
			{
				PC->AddImpulseAtLocation(FVector::ForwardVector * HitForce, ImpactResult.Location, ImpactResult.BoneName);
			}
		}
	}
}


void ATDProjectileBase::SetProjectileConfig(FWeaponConfig NewWeaponConfig, const ATDCharacter* NewCharacter)
{
	WeaponConfig = NewWeaponConfig;
	Character = NewCharacter;
}


FVector ATDProjectileBase::GetProjectileVelocity() const
{
	return ProjectileMC->Velocity;
}


void ATDProjectileBase::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!bDoOverlapDamage)
		return;

	if (Role < ROLE_Authority)
		return;

	FDamageEvent DamageEvent;
	DamageEvent.DamageTypeClass = UDamageType::StaticClass();
	AController* Controller = nullptr;
	ATDCharacter* MyChar = nullptr;


	if (Character.IsValid())
	{
		MyChar = Character.Get();
		Controller = MyChar->GetController();
	}

	if (OtherActor)
	{
		ATDCharacter* OtherChar = Cast<ATDCharacter>(OtherActor);
		if (OtherChar && MyChar)
		{
			// don't damage friends
			if (OtherChar->GetGenericTeamId() == MyChar->GetGenericTeamId())
				return;
		}

		OtherActor->TakeDamage(WeaponConfig.Damage, DamageEvent, Controller, this);
	}

}


void ATDProjectileBase::SetWeapon(ATDWeaponBase* OwnerWeapon)
{
	// FIX ME: wtf?
	Weapon = WeaponOwner = OwnerWeapon;
}


void ATDProjectileBase::OnRep_WeaponOwner()
{
	Weapon = WeaponOwner;
}


void ATDProjectileBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATDProjectileBase, WeaponOwner);
}

