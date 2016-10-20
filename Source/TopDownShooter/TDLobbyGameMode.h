// TopDownShooter project (c) 2016 V.Khmelevskiy

#pragma once

#include "GameFramework/GameMode.h"
#include "TDLobbyGameMode.generated.h"


/**
 * Lobby game mode for managing game parties
 */
UCLASS()
class TOPDOWNSHOOTER_API ATDLobbyGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:

	ATDLobbyGameMode(const FObjectInitializer& ObjectInitilizer);
	
	// used internally to notify all player about connecting players
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// used internally to notify all player about leaving players
	virtual void Logout(AController* Exiting) override;

	// send chat message to all players
	void AddChatMessage(APlayerState* Sender, const FString& Text);

};
