// TopDownShooter project (c) 2016 V.Khmelevskiy

#pragma once

#include "AIController.h"
#include "TDAIController.generated.h"

class ATDMonsterCharacter;
class UBehaviorTreeComponent;
class UBlackboardComponent;
/**
 * 
 */
UCLASS()
class TOPDOWNSHOOTER_API ATDAIController : public AAIController
{
	GENERATED_BODY()
public:
	// Sets default values for this character's properties
	ATDAIController(const FObjectInitializer& ObjectInitializer);
	
	virtual void Possess(APawn* InPawn) override;

	UFUNCTION()
	virtual void OnPawnDie();

	AActor* GetEnemy();
	void SetEnemy(AActor* NewEnemy);


	virtual FGenericTeamId GetGenericTeamId() const override;

protected:

	// TODO: add team id property, ensure it returns with GetGenericTeamId()

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, category = AI)
	UBehaviorTreeComponent* BehaviorTreeComp;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, category = AI)
	UBlackboardComponent* BlackboardComp;

	int32 AttackModeKeyId;
	int32 EnemyKeyId;
	int32 SelfKeyId;

	UPROPERTY()
	TWeakObjectPtr<AActor> MyEnemy;

	UPROPERTY()
	TWeakObjectPtr<ATDMonsterCharacter> MyPawn;

public:
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

};
