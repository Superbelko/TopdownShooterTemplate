// TopDownShooter project (c) 2016 V.Khmelevskiy, S.Zhernovoy

#include "TopDownShooter.h"
#include "TDPlayerCharacter.h"
#include "TDPlayerController.h"
#include "TDSaveGame.h"
#include "HideableMeshActor.h"
#include "TDGameInstance.h"
#include "AI/TDMonsterCharacter.h"

#include "AIController.h"
#include "UnrealNetwork.h"


// teams
#include "TDGameMode.h"


void ATDPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ATDPlayerController::OnPrimaryAttackStart);	
	InputComponent->BindAction("PrimaryAttack", IE_Released, this, &ATDPlayerController::OnPrimaryAttackStop);
	InputComponent->BindAction("SecondaryAttack", IE_Pressed, this, &ATDPlayerController::OnSecondaryAttackStart);
	InputComponent->BindAction("SecondaryAttack", IE_Released, this, &ATDPlayerController::OnSecondaryAttackStop);

	InputComponent->BindAction("QuickLoad", IE_Released, this, &ATDPlayerController::QuickLoad);
}

void ATDPlayerController::PlayerTick(float DeltaSeconds)
{
	Super::PlayerTick(DeltaSeconds);
	
	TraceMouse();
}

void ATDPlayerController::OnPrimaryAttackStart()
{
	if (!PlayerIsAlive())
		return;

	PrimaryAttack(true);

	if (Role < ROLE_Authority)
	{
		ServerOnPrimaryAttack(true);
	}
}

void ATDPlayerController::OnPrimaryAttackStop()
{
	if (!PlayerIsAlive())
		return;

	PrimaryAttack(false);

	if (Role < ROLE_Authority)
	{
		ServerOnPrimaryAttack(false);
	}
}

void ATDPlayerController::PrimaryAttack(bool bNewPrimaryAttack)
{
	if (PlayerIsAlive())
	{
		MyPawn->SetIsPrimaryAttack(bNewPrimaryAttack);
	}
}


bool ATDPlayerController::ServerOnPrimaryAttack_Validate(bool bNewPrimaryAttack)
{
	return true;
}

void ATDPlayerController::ServerOnPrimaryAttack_Implementation(bool bNewPrimaryAttack)
{
	PrimaryAttack(bNewPrimaryAttack);
}

void ATDPlayerController::OnSecondaryAttackStart()
{
	if (!PlayerIsAlive())
		return;

	SecondaryAttack(true);

	if (Role < ROLE_Authority)
	{
		ServerOnSecondaryAttack(true);
	}
}

void ATDPlayerController::OnSecondaryAttackStop()
{
	if (!PlayerIsAlive())
		return;

	SecondaryAttack(false);

	if (Role < ROLE_Authority)
	{
		ServerOnSecondaryAttack(false);
	}

}

void ATDPlayerController::SecondaryAttack(bool bNewSecondaryAttack)
{
	if (PlayerIsAlive())
	{
		MyPawn->SetIsSecondaryAttack(bNewSecondaryAttack);
	}
}


bool ATDPlayerController::ServerOnSecondaryAttack_Validate(bool bNewSomeBool)
{
	return true;
}


void ATDPlayerController::ServerOnSecondaryAttack_Implementation(bool bNewSecondaryAttack)
{
	SecondaryAttack(bNewSecondaryAttack);
}

void ATDPlayerController::Possess(APawn* aPawn)
{
	Super::Possess(aPawn);

	MyPawn = Cast<ATDPlayerCharacter>(aPawn);

	MyPawn->EquipWeapon(1);
}

void ATDPlayerController::QuickLoad()
{
	if (!HasAuthority())
		return;

	ATDGameMode* GameMode = Cast<ATDGameMode>(GetWorld()->GetAuthGameMode());

	if (GameMode)
	{
		UTDGameIntance* GameInstance = Cast<UTDGameIntance>(GetGameInstance());
		GameInstance->LoadedSaveGame = Cast<UTDSaveGameData>(UGameplayStatics::LoadGameFromSlot("checkpoint", 0));

		if (GameMode->NumPlayers == 1)
		{
			RestartLevel();
		}
		else
		{
			FString MapName = GetWorld()->GetLocalURL();
			MapName.ReplaceInline(*GetWorld()->StreamingLevelsPrefix, TEXT(""));
			MapName = MapName + "?listen";

			// TODO: move to more apropriate place, such as game mode
			ATDMonsterCharacter::ResetDropData();

			GetWorld()->ServerTravel(MapName, true);
		}
	}
}

void ATDPlayerController::TraceMouse()
{	
	if (auto Char = Cast<ATDPlayerCharacter>(GetPawn()))
	{
		FPlane XYPlane = FPlane(Char->GetActorLocation(), FVector::UpVector);
		FVector PlayerViewPoint;
		FRotator PlayerViewRot;
		FVector MouseWorldLoc;
		FVector MouseWorldDir;
		FVector CrossPoint = FVector();

		// get mouse pos in world3D
		DeprojectMousePositionToWorld(MouseWorldLoc, MouseWorldDir);
		// get eyes position in world3d
		GetPlayerViewPoint(PlayerViewPoint, PlayerViewRot);
	
		CrossPoint = FMath::LinePlaneIntersection(PlayerViewPoint, MouseWorldLoc, XYPlane);

		if (!CrossPoint.IsZero())
		{
			Char->SetMouseHit(CrossPoint);
		}
		else
		{
			FHitResult OurHitResult;
			GetHitResultUnderCursor(ECC_Visibility, true, OurHitResult);
			Char->SetMouseHit(OurHitResult.Location);
		}
	}
}

void ATDPlayerController::BeginCameraOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//HiddenActors.AddUnique(OtherActor);
	ITDHideableMeshInterface* hmesh = Cast<ITDHideableMeshInterface>(OtherActor);
	if (hmesh)
		ITDHideableMeshInterface::Execute_OnBecomeTranslucent(OtherActor);

}

void ATDPlayerController::EndCameraOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//HiddenActors.RemoveSingle(OtherActor);
	ITDHideableMeshInterface* hmesh = Cast<ITDHideableMeshInterface>(OtherActor);
	if ( hmesh )
		ITDHideableMeshInterface::Execute_OnBecomeOpaque(OtherActor);
}

bool ATDPlayerController::PlayerIsAlive() const
{
	return MyPawn && !MyPawn->IsDead();
}



FGenericTeamId ATDPlayerController::GetGenericTeamId() const
{
	return TeamIdPlayer;
}

ETeamAttitude::Type ATDPlayerController::GetTeamAttitudeTowards(const AActor& Other) const
{
	FGenericTeamId OtherTeam = TeamIdHostileToAll;

	if (auto AICon = Cast<AAIController>(Other.GetInstigatorController()))
	{
		OtherTeam = AICon->GetGenericTeamId();
	}

	if (OtherTeam == TeamIdFriendlyToAll || OtherTeam == GetGenericTeamId())
		return ETeamAttitude::Friendly;

	if (OtherTeam == TeamIdNeutralToAll)
		return ETeamAttitude::Neutral;

	return ETeamAttitude::Hostile;
}

void ATDPlayerController::UnPossess()
{
	Super::UnPossess();

	MyPawn = nullptr;
}

void ATDPlayerController::SetupCameraVolumeBlocking(UPrimitiveComponent* Component) const
{
	// don't bind for other's player characters
	if (!IsLocalPlayerController())
		return;

	Component->OnComponentBeginOverlap.AddDynamic(this, &ATDPlayerController::BeginCameraOverlap);
	Component->OnComponentEndOverlap.AddDynamic(this, &ATDPlayerController::EndCameraOverlap);
}

void ATDPlayerController::PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel)
{
	Super::PreClientTravel(PendingURL, TravelType, bIsSeamlessTravel);

	if (GetWorld())
	{
		UE_LOG(LogTemp, Log, TEXT("Show loading screen"))
		UTDGameIntance* GameInstance = Cast<UTDGameIntance>(GetGameInstance());

		if ( GameInstance )
			GameInstance->ShowLoadingScreen();
	}
}

void ATDPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	MyPawn = Cast<ATDPlayerCharacter>(GetPawn());
}

void ATDPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	MyPawn = Cast<ATDPlayerCharacter>(GetPawn());
}
