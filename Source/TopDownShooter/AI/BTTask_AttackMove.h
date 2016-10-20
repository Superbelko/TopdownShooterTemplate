// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "AITypes.h"
#include "BTTask_AttackMove.generated.h"

class ATDAIController;
class ATDMonsterCharacter;
/**
 * Same as MoveTo task but also does attack while moving
 */
UCLASS()
class TOPDOWNSHOOTER_API UBTTask_AttackMove : public UBTTask_MoveTo
{
	GENERATED_UCLASS_BODY()

public:
	/** Ability used to attack while moving */
	UPROPERTY(Category = Node, EditAnywhere)
	FString AttackAbility;

	
private:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY()
	ATDAIController* MonsterAI;

	UPROPERTY()
	ATDMonsterCharacter* MonsterPawn;
};
