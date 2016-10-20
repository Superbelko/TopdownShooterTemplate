// TopDownShooter project (c) 2016 V.Khmelevskiy

#include "TopDownShooter.h"
#include "TDPlayerState.h"

#include "TDLobbyGameMode.h"
#include "TDGameMode.h"

#include "UnrealNetwork.h"


ATDPlayerState::ATDPlayerState(const FObjectInitializer& ObjectInitilizer)
	:Super(ObjectInitilizer)
{

}

void ATDPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATDPlayerState, bIsReady);
}

void ATDPlayerState::SetUserName(const FString& NewName)
{
	if (NewName.Len() == 0)
		return;

	//PlayerName = NewName;
	SetPlayerName(NewName);
	if (!HasAuthority())
	{
		Server_SetPlayerName(NewName);
	}
}

void ATDPlayerState::SetIsReady(bool bNewReady)
{
	bIsReady = bNewReady;

	if (!HasAuthority())
	{
		Server_SetIsReady(bNewReady);
	}
}

void ATDPlayerState::SendChatMessage(const FString& Text)
{
	if (HasAuthority())
	{
		ATDLobbyGameMode* Lobby = GetWorld()->GetAuthGameMode<ATDLobbyGameMode>();
		if ( Lobby )
		{
			Lobby->AddChatMessage(this, Text.Left(MAX_MESSAGE_LEN));
			return;
		}

		// TODO: unified logic for lobby & game modes
		ATDGameMode* Game = GetWorld()->GetAuthGameMode<ATDGameMode>();
		if ( Game )
		{ 
		}
	}
	else
	{
		Server_SendChatMessage(Text.Left(MAX_MESSAGE_LEN));
	}
}

bool ATDPlayerState::Server_SetIsReady_Validate(bool bNewReady)
{
	return true;
}

void ATDPlayerState::Server_SetIsReady_Implementation(bool bNewReady)
{
	bIsReady = bNewReady;
}

void ATDPlayerState::Client_ReceiveChatMessage_Implementation(ATDPlayerState* Sender, const FString& Text)
{
	OnChatMessage.Broadcast(Sender, Text);
}

bool ATDPlayerState::Server_SendChatMessage_Validate(const FString& Text)
{
	return true;
}


void ATDPlayerState::Server_SendChatMessage_Implementation(const FString& Text)
{
	ATDLobbyGameMode* Lobby = GetWorld()->GetAuthGameMode<ATDLobbyGameMode>();
	if (Lobby)
	{
		Lobby->AddChatMessage(this, Text.Left(MAX_MESSAGE_LEN));
		return;
	}
}

bool ATDPlayerState::Server_SetPlayerName_Validate(const FString& NewName)
{
	return true;
}


void ATDPlayerState::Server_SetPlayerName_Implementation(const FString& NewName)
{
	if (NewName.Len() == 0)
		return;

	SetPlayerName(NewName);
}

void ATDPlayerState::Client_PlayerLeaves_Implementation(APlayerState* OldPlayer)
{
	OnPlayerLeaves.Broadcast(OldPlayer);
}

void ATDPlayerState::Client_PlayerJoined_Implementation(APlayerState* NewPlayer)
{
	OnPlayerJoined.Broadcast(NewPlayer);
}