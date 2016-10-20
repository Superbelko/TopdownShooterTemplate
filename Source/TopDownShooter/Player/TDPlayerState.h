// TopDownShooter project (c) 2016 V.Khmelevskiy

#pragma once

#include "GameFramework/PlayerState.h"
#include "TDPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerJoined, APlayerState*, NewPlayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerLeaves, APlayerState*, ExitingPlayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChatMessage, APlayerState*, Sender, const FString&, Text);


/**
 * 
 */
UCLASS()
class TOPDOWNSHOOTER_API ATDPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	// maximum chat message length
	static const int32 MAX_MESSAGE_LEN = 300;

public:
	
	ATDPlayerState(const FObjectInitializer& ObjectInitilizer);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// SetPlayerName is already exists in base class
	UFUNCTION(BlueprintCallable, Category = Player)
	void SetUserName(const FString& NewName);

	UFUNCTION(BlueprintCallable, Category = Player)
	void SetIsReady(bool bIsReady);

	UFUNCTION(BlueprintCallable, Category = Chat)
	void SendChatMessage(const FString& Text);

	UFUNCTION(Client, Unreliable, Category = Online)
	void Client_ReceiveChatMessage(ATDPlayerState* Sender, const FString& Text);

	UFUNCTION(Client, Unreliable, Category = Online)
	void Client_PlayerJoined(APlayerState* NewPlayer);

	UFUNCTION(Client, Unreliable, Category = Online)
	void Client_PlayerLeaves(APlayerState* OldPlayer);

public:
	UPROPERTY(BlueprintAssignable, Category = Online)
	FOnPlayerJoined OnPlayerJoined;

	UPROPERTY(BlueprintAssignable, Category = Online)
	FOnPlayerLeaves OnPlayerLeaves;

	UPROPERTY(BlueprintAssignable, Category = Chat)
	FOnChatMessage OnChatMessage;

protected:
	UFUNCTION(BlueprintCallable, Unreliable, Server, WithValidation, Category = Player)
	void Server_SetIsReady(bool bNewReady);

	UFUNCTION(BlueprintCallable, Unreliable, Server, WithValidation, Category = Player)
	void Server_SetPlayerName(const FString& NewName);

	UFUNCTION(BlueprintCallable, Unreliable, Server, WithValidation, Category = Chat)
	void Server_SendChatMessage(const FString& Text);

protected:
	UPROPERTY(BlueprintReadOnly, Replicated, Category = Player)
	bool bIsReady;
};
