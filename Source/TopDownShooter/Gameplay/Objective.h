// TopDownShooter project (c) 2016 V.Khmelevskiy

#pragma once

#include "GameFramework/Actor.h"
#include "Objective.generated.h"


UENUM(Blueprintable)
enum class EObjectiveStatus : uint8
{
	Undefined,
	Success,
	Failed
};


/** 
* Objective tracking actor. Place one where you want to put mission objective.
* Assumes deactivated state by default.
*/
UCLASS()
class TOPDOWNSHOOTER_API AObjective : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AObjective(const FObjectInitializer& ObjectInitializer);


	// =============== EVENTS =====================
	/** Received when objective becomes active */
	UFUNCTION(BlueprintImplementableEvent, Category = Objective)
	void OnBecomeActive();

	/** Received when objective becomes inactive(finished/failed) */
	UFUNCTION(BlueprintImplementableEvent, Category = Objective)
	void OnBecomeInactive(EObjectiveStatus Status);


	// =============== METHODS =====================

	UFUNCTION(BlueprintCallable, Category = Objective)
	void SetIsActive(bool bNewActive, EObjectiveStatus Status = EObjectiveStatus::Undefined);


protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere ,Category = Details)
	FText Description;

private:
	UPROPERTY()
	UBillboardComponent* Billboard;
};
