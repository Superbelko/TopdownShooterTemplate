// TopDownShooter project (c) 2016 V.Khmelevskiy

#include "TopDownShooter.h"
#include "TDCharacter.h"
#include "Player/TDCharacterMovement.h"


UTDCharacterMovement::UTDCharacterMovement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


float UTDCharacterMovement::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	// TODO: add monster speed handling for better movement
	const ATDCharacter* CharacterOwner = Cast<ATDCharacter>(PawnOwner);
	if (CharacterOwner)
	{
		if (CharacterOwner->IsRunning())
		{
			MaxSpeed *= CharacterOwner->GetRunningSpeedModifier();
		}
	}

	return MaxSpeed;
}
