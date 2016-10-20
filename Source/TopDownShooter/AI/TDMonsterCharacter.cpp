// TopDownShooter project (c) 2016 V.Khmelevskiy

#include "TopDownShooter.h"
#include "TDAIController.h"
#include "TDMonsterCharacter.h"
#include "TDGameMode.h"
#include "Player/TDPlayerCharacter.h"
#include "Gameplay/TDPickUp.h"

// includes AI
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"

#include "Projectile/TDProjectileBase.h"

#include "UnrealNetwork.h"

#include "TDGameMode.h"


// inited in begin play
static TArray<FLootDropData> dropList;
static bool bIsDropInitialized = false;


ATDMonsterCharacter::ATDMonsterCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("SightConf", true);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->LoseSightRadius = 2000.0f;
	SightConfig->SightRadius = 3500.0f;
	SightConfig->PeripheralVisionAngleDegrees = 60.0f;
	//SightConfig->MaxAge = 1.0f;

	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>("HearingConf", true);
	HearingConfig->HearingRange = 2500.0f;
	HearingConfig->LoSHearingRange = 3000.0f;
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = false;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;
	//HearingConfig->MaxAge = 2.0f;

	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	PerceptionComponent->SetDominantSense(SightConfig->Implementation);
	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->ConfigureSense(*HearingConfig);

	TeamId = TeamIdMonster;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ATDMonsterCharacter::InitDropData()
{
	ATDGameMode* GameMode = Cast<ATDGameMode>(GetWorld()->GetAuthGameMode());

	// ====================== FILL DROP LIST ===============================

	// 30% health drop _IF_ health low
	dropList.Add(FLootDropData(GameMode->Drop_HealthPickup, FVector2D(0.3f, 0.6f),
		[&](ATDPlayerCharacter* player) -> bool { return player->IsLowHealth(); }
	));

	dropList.Add(FLootDropData(GameMode->Drop_ArmorPickup, FVector2D(0.13f, 0.18f))); // low chance for armor
	dropList.Add(FLootDropData(GameMode->Drop_HealthPickup, FVector2D(0.96f, 1.0f))); // low chance for health

																					  // 10% chance for all ammo types
	dropList.Add(FLootDropData(GameMode->Drop_AmmoPickups[0], FVector2D(0.26f, 0.36f)));
	dropList.Add(FLootDropData(GameMode->Drop_AmmoPickups[1], FVector2D(0.01f, 0.11f)));
	dropList.Add(FLootDropData(GameMode->Drop_AmmoPickups[2], FVector2D(0.52f, 0.62f)));
	dropList.Add(FLootDropData(GameMode->Drop_AmmoPickups[3], FVector2D(0.43f, 0.53f)));
	dropList.Add(FLootDropData(GameMode->Drop_AmmoPickups[4], FVector2D(0.88f, 0.98f)));

	bIsDropInitialized = true;
	// =====================================================================	
}

void ATDMonsterCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AICon = Cast<ATDAIController>(NewController);
}

void ATDMonsterCharacter::Die()
{
	Super::Die();

	if (AICon)
	{
		AICon->OnPawnDie();
	}

	if (HasAuthority())
		DropLoot();

}

void ATDMonsterCharacter::OnUpdatePerception(TArray<AActor*> UpdatedActors)
{

}

void ATDMonsterCharacter::OnActorPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus)
{
	if (!AICon)
		return;

	// ignore self
	if (Actor == this)
		return;

	// ignore any spectators
	if (Cast<ASpectatorPawn>(Actor))
		return;

	ATDCharacter* CharEnemy = Cast<ATDCharacter>(Actor);
	if (CharEnemy)
	{
		if (AICon->GetEnemy() == CharEnemy && CharEnemy->IsDead())
		{
			// TODO: move to proper place
			bIsAttacking = false;

			AICon->SetEnemy(nullptr);
			return;
		}
	}

	if (AICon->GetEnemy())
		return;

	AICon->SetEnemy(Actor);

}

void ATDMonsterCharacter::ForceUpdatePerception()
{
	PerceptionComponent->RequestStimuliListenerUpdate();

	if (AICon)
	{
		if (ATDCharacter* Enemy = Cast<ATDCharacter>(AICon->GetEnemy()))
		{
			if (Enemy->IsDead() || Enemy == this)
				AICon->SetEnemy(nullptr);
		}
	}
}

void ATDMonsterCharacter::DamageActorsInVolume(UBoxComponent* VolumeActor, FName AttackModeName)
{
	TSet<AActor*> Actors;
	
	if (!VolumeActor || AttackModeName.IsNone())
		return;
		
	int32 id = HasAttackModeName(AttackModeName);

	if (id == -1)
		return;

	auto AttackMode = AttackModes[id];

	VolumeActor->GetOverlappingActors(Actors);

	FDamageEvent DamageEvent;
	DamageEvent.DamageTypeClass = UDamageType::StaticClass();

	for (AActor* Actor : Actors)
	{
		// do not hit self
		if ( Actor == this )
			continue;


		Actor->TakeDamage(AttackMode.Damage, DamageEvent, GetController(), this);
	}
}

void ATDMonsterCharacter::DropLoot() const
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATDPlayerCharacter::StaticClass(), Actors);

	// umm... magic
	FMath::RandInit(FDateTime::UtcNow().GetTicks() / 3001);

	for (AActor* Actor : Actors)
	{
		ATDPlayerCharacter* player = Cast<ATDPlayerCharacter>(Actor);

		// iterate drop list and roll drop for player
		for (auto& RollDropItemFor : dropList)
		{
			RollDropItemFor(player, GetWorld(), GetActorTransform());
		}
	}
}

void ATDMonsterCharacter::DropItem(TSubclassOf<ATDPickUp> ItemPickup, ATDPlayerCharacter* Player) const
{
	AActor* actor = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), ItemPickup, GetActorTransform(), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, Player);
	ATDPickUp* pickup = Cast<ATDPickUp>(actor);

	if (pickup)
	{
		pickup->MakePersonal(Player);
		UGameplayStatics::FinishSpawningActor(pickup, GetActorTransform());
	}
}

void ATDMonsterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATDMonsterCharacter, bIsAttacking);
	DOREPLIFETIME(ATDMonsterCharacter, CurrentAttackMode);
}

int32 ATDMonsterCharacter::HasAttackMode(const FAttackMode& AttackMode) const
{
	int32 id = INDEX_NONE;
	AttackModes.Find(AttackMode, id);
	return id;
}

int32 ATDMonsterCharacter::HasAttackModeName(FName AttackName) const
{
	FAttackMode am;
	am.Name = AttackName;
	return HasAttackMode(am);
}

bool ATDMonsterCharacter::CanAttackEnemyWithMode(const AActor* const Enemy, const FAttackMode& AMode, int32& OutModeIndex) const
{
	if (!Enemy)
		return false;

	OutModeIndex = HasAttackMode(AMode);
	if (INDEX_NONE == OutModeIndex)
		return false;

	bool bIsReady = AttackModeLastTime[OutModeIndex] + AttackModes[OutModeIndex].Cooldown < GetWorld()->TimeSeconds;
	if (!bIsReady)
		return false;

	float MinRange = AttackModes[OutModeIndex].MinRange;
	float MaxRange = AttackModes[OutModeIndex].MaxRange;
	float Distance = FVector::Dist(GetActorLocation(), Enemy->GetActorLocation());

	return FMath::IsWithinInclusive<float>(Distance, MinRange, MaxRange);
}

bool ATDMonsterCharacter::CanAttackEnemyWithModeName(const AActor* const Enemy, FName ModeName, int32& OutModeIndex) const
{
	FAttackMode am = FAttackMode();
	am.Name = ModeName;

	return CanAttackEnemyWithMode(Enemy, am, OutModeIndex);
}

void ATDMonsterCharacter::AttackEnemyWithMode(const AActor* const Enemy, const FAttackMode& AMode)
{
	int32 index = INDEX_NONE;
	
	if (!CanAttackEnemyWithMode(Enemy, AMode, index))
		return;

	if (INDEX_NONE == index)
		return;

	// ===========================================================
	// NOTE: below this line we need to use actual attack mode config instead method parameter

	const FAttackMode& Mode = AttackModes[index];

	if ( Mode.ProjectileClass )
	{
		float ProjectileVelocity = Mode.ProjectileClass->GetDefaultObject<ATDProjectileBase>()->GetProjectileVelocity().X;
		FVector PredictedLoc = GetPredictionLocation(Enemy, ProjectileVelocity);

		if (PredictedLoc.IsZero())
			return;

		FireProjectile(Mode.ProjectileClass, RangedSocketName, PredictedLoc, Mode.Damage);
	}
	else // non-projectile abilities
	{

	}

	SetAttackModeCooldown(Mode);
}

void ATDMonsterCharacter::AttackEnemyWithModeName(const AActor* const Enemy, FName ModeName)
{
	FAttackMode am = FAttackMode();
	am.Name = ModeName;

	AttackEnemyWithMode(Enemy, am);
}

void ATDMonsterCharacter::SetAttackModeCooldown(const FAttackMode& AttackMode)
{
	int32 index = HasAttackMode(AttackMode);
	if (INDEX_NONE == index)
		return;

	AttackModeLastTime[index] = GetWorld()->GetTimeSeconds();
}

void ATDMonsterCharacter::SetAttackModeNameCooldown(FName AttackName)
{
	FAttackMode am = FAttackMode();
	am.Name = AttackName;

	SetAttackModeCooldown(am);
}

void ATDMonsterCharacter::SetCurrentAttackMode(FName NewAttackMode)
{
	CurrentAttackMode = NewAttackMode;
}

float ATDMonsterCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float damage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	
	if ( damage && AICon && !AICon->GetEnemy())
	{
		// set pawn as enemy, ignore non-pawn actors
		if (auto Pawn = Cast<APawn>(DamageCauser))
			AICon->SetEnemy(Pawn);
	}

	return damage;
}

void ATDMonsterCharacter::ResetDropData()
{
	dropList.Empty();
	bIsDropInitialized = false;
}

#if 0
void ATDMonsterCharacter::WriteActorData(FArchive& Ar)
{
	FString aName = GetName();
	//Ar << 
}
#endif

void ATDMonsterCharacter::SetIsAttacking(bool bAttacking)
{
	bIsAttacking = bAttacking;
}

bool ATDMonsterCharacter::GetIsAttacking() const
{
	return bIsAttacking;
}

void ATDMonsterCharacter::OnAttackHit_Implementation(FName AttackModeName /*= NAME_None*/)
{

}

void ATDMonsterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &ATDMonsterCharacter::OnUpdatePerception);
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ATDMonsterCharacter::OnActorPerceptionUpdate);

	AttackModeLastTime.SetNumZeroed(AttackModes.Num());
}

void ATDMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();

	UAIPerceptionSystem* PercSys = UAIPerceptionSystem::GetCurrent(GetWorld());

	if (!HasAuthority())
		return;

	PercSys->RegisterSenseClass(SightConfig->Implementation);
	PercSys->RegisterSenseClass(HearingConfig->Implementation);

	GetWorldTimerManager().SetTimer(PercUpdateTimer, this, &ATDMonsterCharacter::ForceUpdatePerception, 0.4f, true, 0.1f);

	if (bIsDropInitialized)
		return;

	InitDropData();
}

void FLootDropData::operator()(ATDPlayerCharacter* Owner, UWorld* WorldContext, const FTransform& Transform)
{
	float roll = FMath::FRand();
	if (Chance.X <= roll && roll <= Chance.Y)
	{
		if (DropCondition)
		{
			// do checks on player
			if (!DropCondition(Owner))
				return;
		}

		AActor* actor = UGameplayStatics::BeginDeferredActorSpawnFromClass(WorldContext, Item, Transform, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, Owner);
		ATDPickUp* pickup = Cast<ATDPickUp>(actor);

		if (pickup)
		{
			pickup->MakePersonal(Owner);
			UGameplayStatics::FinishSpawningActor(pickup, Transform);
		}
	}
}

FLootDropData::FLootDropData(TSubclassOf<ATDPickUp> ItemClass, FVector2D ChanceRange)
	:Item(ItemClass), Chance(ChanceRange)
{

}

FLootDropData::FLootDropData(TSubclassOf<ATDPickUp> ItemClass, FVector2D ChanceRange, TFunction<bool(ATDPlayerCharacter*)> DropConditionDelegate)
	: Item(ItemClass), Chance(ChanceRange), DropCondition(DropConditionDelegate)
{

}
