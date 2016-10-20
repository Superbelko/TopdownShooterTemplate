// TopDownShooter project (c) 2016 V.Khmelevskiy

#include "TopDownShooter.h"
#include "Player/TDPlayerCharacter.h"
#include "Player/TDItem.h"
#include "TDPickUp.h"

#include "UnrealNetwork.h"


// Sets default values
ATDPickUp::ATDPickUp(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	// add   Sphere
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));

	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionObjectType(COLLISION_PICKUP);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// set our root
	SetRootComponent(Sphere);

	// add   ProjectileMesh
	ProjectileFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ProjectileFX"));
	ProjectileFX->SetupAttachment(Sphere);

	bReplicates = true;
}

// Called when the game starts or when spawned
void ATDPickUp::BeginPlay()
{
	Super::BeginPlay();
	
}


void ATDPickUp::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if ( ATDPlayerCharacter* PlayerCharacter = Cast<ATDPlayerCharacter>(OtherActor))
	{
		// if owner is set only owner can pickup item
		if (GetOwner() && GetOwner() != OtherActor)
			return;

		OnPickUp(PlayerCharacter);
	}
}

void ATDPickUp::OnPickUp(ATDPlayerCharacter* Player)
{

}

void ATDPickUp::RemovePickUp()
{
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(0.01f);
}

void ATDPickUp::MakePersonal(ATDPlayerCharacter* OwnerPlayer)
{
	SetOwner(OwnerPlayer);

	// manual call on server
	OnRep_IsPersonal();

	bIsPersonal = true;
	//Sphere->SetSphereRadius(200);
	
	// docs says need to manually close channels for replication, skip for now
	//bOnlyRelevantToOwner = true;
}

void ATDPickUp::OnRep_IsPersonal()
{
	SetLifeSpan(60.0f);
	ProjectileFX->SetOnlyOwnerSee(true);
	Sphere->SetOnlyOwnerSee(true);
}

void ATDPickUp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATDPickUp, bIsPersonal);
}

void ATDHealthPickUp::OnPickUp(ATDPlayerCharacter* Player)
{
	if (!Player->IsFullHealth())
	{
		Player->AddHealth(Health);

		RemovePickUp();
	}
}

void ATDArmorPickUp::OnPickUp(ATDPlayerCharacter* Player)
{
	if (!Player->IsFullArmor())
	{
		Player->AddArmor(Armor);

		RemovePickUp();
	}
}

void ATDItemPickUp::OnPickUp(ATDPlayerCharacter* Player)
{
	if (Player->CanPickUpItem(Item))
	{
		Player->InventoryAddItem(Item, ItemCount);

		RemovePickUp();
	}
}
