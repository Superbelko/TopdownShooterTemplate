// Fill out your copyright notice in the Description page of Project Settings.

#include "TopDownShooter.h"
#include "BTTask_AttackMove.h"

#include "TDAIController.h"
#include "TDMonsterCharacter.h"


UBTTask_AttackMove::UBTTask_AttackMove(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Move and Attack";
}

void UBTTask_AttackMove::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (MonsterAI && MonsterPawn)
	{
		if (auto enemy = MonsterAI->GetEnemy())
		{
			int32 unused;
			if (MonsterPawn->CanAttackEnemyWithModeName(enemy, *AttackAbility, unused))
			{
				MonsterPawn->AttackEnemyWithModeName(enemy, *AttackAbility);
				MonsterPawn->SetCurrentAttackMode(*AttackAbility);
			}
		}
	}
}

EBTNodeResult::Type UBTTask_AttackMove::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	MonsterAI = Cast<ATDAIController>(OwnerComp.GetAIOwner());
	if (MonsterAI)
	{
		MonsterPawn = Cast<ATDMonsterCharacter>(MonsterAI->GetPawn());
	}

	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
