// TopDownShooter project (c) 2016 V.Khmelevskiy

#include "TopDownShooter.h"

#include "UnrealNetwork.h"
#include "Perception/AIPerceptionSystem.h"

#include "TDCharacter.h"
#include "Effects/TDImpactEffect.h"
#include "Projectile/TDProjectileBase.h"
#include "Player/TDCharacterMovement.h"

// teams
#include "TDPlayerController.h"
#include "AI/TDAIController.h"
#include "TDGameMode.h"


// Sets default values
ATDCharacter::ATDCharacter(const FObjectInitializer& ObjectInitializer ) 
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UTDCharacterMovement>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TeamId = TeamIdNeutralToAll;
}

void ATDCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATDCharacter, Health);
	DOREPLIFETIME(ATDCharacter, bIsRunning);
	DOREPLIFETIME(ATDCharacter, TeamId);
}

float ATDCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Damage > Health)
	{
		Damage = Health;
	}

	Health -= Damage;

	if (Health <= 0.f)
	{
		Die();
	}
	return Damage;
}

void ATDCharacter::Die()
{
	static FName CollisionProfileName(TEXT("Ragdoll"));

	if (HasAuthority())
	{
		UAIPerceptionSystem::GetCurrent(GetWorld())->UnregisterSource(*this);
	}

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GetCapsuleComponent()->SetVisibility(false);

	SetActorEnableCollision(true);
	GetMesh()->SetCollisionProfileName(CollisionProfileName);
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;

	SetLifeSpan(10);

	OnDeath();
}

float ATDCharacter::GetMaxHealth() const
{
	return GetClass()->GetDefaultObject<ATDCharacter>()->Health;
}

bool ATDCharacter::IsDead() const
{
	return Health <= 0;
}

void ATDCharacter::OnRep_Health()
{
	if (Health <= 0.0f)
	{
		Die();
	}
}

FGenericTeamId ATDCharacter::GetGenericTeamId() const
{
	return TeamId;
}

ETeamAttitude::Type ATDCharacter::GetTeamAttitudeTowards(const AActor& Other) const
{
	ATDAIController *AICon = Cast<ATDAIController>(GetController());
	ATDPlayerController *PC = Cast<ATDPlayerController>(GetController());
	if (AICon)
	{
		return AICon->GetTeamAttitudeTowards(Other);
	}
	
	if (PC)
	{
		return PC->GetTeamAttitudeTowards(Other);
	}

	return ETeamAttitude::Neutral;
}

void ATDCharacter::FireProjectile(TSubclassOf<ATDProjectileBase> ProjClass, FName SocketName, FVector TargetLoc, float Damage) const
{
	FVector StartLoc = GetMesh()->GetSocketLocation(SocketName);
	FRotator Rot = (TargetLoc - StartLoc).Rotation();
	FTransform Transform = FTransform(Rot, StartLoc);


	ATDProjectileBase* Projectile = GetWorld()->SpawnActor<ATDProjectileBase>(ProjClass, Transform);
	
	FWeaponConfig wconf; 
	wconf.Damage = Damage;

	Projectile->SetProjectileConfig(wconf, this);
}

FVector ATDCharacter::GetPredictionLocation(const AActor* Target, float Velocity)
{
	if (!Target || Velocity == 0)
		return FVector::ZeroVector;

	FVector SLoc = GetActorLocation(); // projectile start loc
	FVector TLoc = Target->GetActorLocation(); // target loc
	FVector Tv = Target->GetVelocity(); // target velocity
	FVector TLoc2 = TLoc + Tv; // target location after 1 sec assuming no velocity change
	float Dist2 = FVector::Dist(SLoc, TLoc2); // distance from bullet to estimated target location
	float dt = Dist2 / Velocity; // time required for projectile to reach estimated location

	// where to shoot to hit moving target (assuming zero acceleration)
	FVector TLoc3 = TLoc + (Tv * dt); // 1/dt ?

	return dt < 0 ? FVector::ZeroVector : TLoc3;
}

#if 0
void ATDCharacter::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	if (Ar.ArIsSaveGame)
	{
		Ar << Health;
	}
}
#endif

// Called when the game starts or when spawned
void ATDCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATDCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void ATDCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

void ATDCharacter::AddHealth(float HealthToAdd)
{
	Health += HealthToAdd;
	Health = FMath::Clamp<float>(Health, 0, GetMaxHealth());
}

bool ATDCharacter::IsFullHealth()
{
	return Health >= GetMaxHealth();
}

void ATDCharacter::SpawnImpactEffects(const FHitResult& Impact, TSubclassOf<ATDImpactEffect> ImpactTemplate)
{
	if (ImpactTemplate && Impact.bBlockingHit)
	{
		FHitResult UseImpact = Impact;

		FTransform const SpawnTransform(Impact.ImpactNormal.Rotation(), Impact.ImpactPoint);
		ATDImpactEffect* EffectActor = GetWorld()->SpawnActorDeferred<ATDImpactEffect>(ImpactTemplate, SpawnTransform);
		if (EffectActor)
		{
			EffectActor->SurfaceHit = UseImpact;
			UGameplayStatics::FinishSpawningActor(EffectActor, SpawnTransform);
		}
	}
}

bool ATDCharacter::IsLowHealth() const
{
	return (Health / GetMaxHealth()) < LowHealthRatio;
}
