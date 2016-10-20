// TopDownShooter project (c) 2016 V.Khmelevskiy

#pragma once

#include "GameFramework/Actor.h"
#include "TDInteractable.generated.h"

/**
* Interactive actor that player can interact with.
*/
UINTERFACE()
class UTDInteractable : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};


class ITDInteractable
{
	GENERATED_IINTERFACE_BODY()

public:
	/** Activates this interactable object
	* Instigator - actor caused activation
	*/
	UFUNCTION(BlueprintImplementableEvent, Category=Interaction)
	void OnInteract(AActor* Instigator);
};
