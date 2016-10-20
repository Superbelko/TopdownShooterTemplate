// TopDownShooter project (c) 2016 V.Khmelevskiy

#pragma once

#include "GameFramework/GameState.h"
#include "TDGameState.generated.h"


class AObjective;

/**
 * 
 */
UCLASS()
class TOPDOWNSHOOTER_API ATDGameState : public AGameState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = LevelState)
	FString GetValueString(const FString& Key) const;

	UFUNCTION(BlueprintCallable, Category = LevelState)
	void SetValueString(const FString& Key, const FString& Data);


	virtual void Serialize(FArchive& Ar) override;

	/** Sets current objective, adds to objectives list if needed */
	UFUNCTION(BlueprintCallable, Category = Objective)
	void SetObjective(AObjective* NewObjective);

private:
	UFUNCTION()
	void OnRep_PlayerKilled();
	
	UFUNCTION()
	void OnRep_ObjectiveChanged();
public:
	// Is one or more players are killed?
	// (UMG use this to show message when player killed)
	UPROPERTY(ReplicatedUsing=OnRep_PlayerKilled, BlueprintReadOnly, VisibleAnywhere, Category = Game)
	bool bPlayerKilled;

	// last checkpoint
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = Game)
	AActor* CheckPoint;

	UPROPERTY(ReplicatedUsing = OnRep_ObjectiveChanged, BlueprintReadOnly, VisibleAnywhere, Category = Objective)
	AObjective* CurrentObjective;


private:

	// Lists all objectives taken and their status
	UPROPERTY()
	TArray<AObjective*> Objectives;

	UPROPERTY()
	TMap<FString, FString> LevelData;
	
};
