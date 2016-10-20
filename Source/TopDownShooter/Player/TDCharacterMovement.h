// TopDownShooter project (c) 2016 V.Khmelevskiy

#pragma once
#include "GameFramework/CharacterMovementComponent.h"
#include "TDCharacterMovement.generated.h"

UCLASS()
class UTDCharacterMovement : public UCharacterMovementComponent
{
	GENERATED_UCLASS_BODY()

	virtual float GetMaxSpeed() const override;
};

