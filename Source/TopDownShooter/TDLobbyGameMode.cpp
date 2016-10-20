// TopDownShooter project (c) 2016 V.Khmelevskiy

#include "TopDownShooter.h"
#include "TDLobbyGameMode.h"
#include "Player/TDPlayerState.h"



ATDLobbyGameMode::ATDLobbyGameMode(const FObjectInitializer& ObjectInitilizer)
	:Super(ObjectInitilizer)
{
	PlayerStateClass = ATDPlayerState::StaticClass();

}


void ATDLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	APlayerState* JoinedPlayerState = nullptr;

	if (NewPlayer->GetPawnOrSpectator())
	{
		JoinedPlayerState = NewPlayer->GetPawnOrSpectator()->PlayerState;
	}

	for (APlayerState* Player : GetGameState<AGameState>()->PlayerArray)
	{
		ATDPlayerState* PlayerState = Cast<ATDPlayerState>(Player);
		if (PlayerState)
		{
			// TODO: what to do if player state not exists?
			PlayerState->Client_PlayerJoined(JoinedPlayerState);
		}
	}
}


void ATDLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	APlayerState* ps = nullptr;

	if (Exiting->GetPawn())
	{
		ps = Exiting->GetPawn()->PlayerState;
	}

	for (APlayerState* Player : GetGameState<AGameState>()->PlayerArray)
	{
		ATDPlayerState* PlayerState = Cast<ATDPlayerState>(Player);
		if (PlayerState)
		{
			PlayerState->Client_PlayerLeaves(ps);
		}
	}
}


void ATDLobbyGameMode::AddChatMessage(APlayerState* Sender, const FString& Text)
{
	for (APlayerState* Player : GetGameState<AGameState>()->PlayerArray)
	{
		ATDPlayerState* PlayerState = Cast<ATDPlayerState>(Player);
		if (PlayerState)
		{
			// can be null but who cares?
			PlayerState->Client_ReceiveChatMessage( Cast<ATDPlayerState>(Sender), Text);
		}
	}
}