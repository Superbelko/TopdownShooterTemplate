// TopDownShooter project (c) 2016 V.Khmelevskiy

#include "TopDownShooter.h"
#include "TDAIController.h"
#include "AI/TDMonsterCharacter.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"

#include "Player/TDPlayerCharacter.h"
#include "Player/TDPlayerController.h"

// team info
#include "TDGameMode.h"




ATDAIController::ATDAIController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));

	BrainComponent = BehaviorTreeComp = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));
}

void ATDAIController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);

	MyPawn = Cast<ATDMonsterCharacter>(InPawn);

	if (MyPawn.IsValid() && MyPawn->BehaviorTree)
	{
		if (MyPawn->BehaviorTree->BlackboardAsset)
		{
			BlackboardComp->InitializeBlackboard(*MyPawn->BehaviorTree->BlackboardAsset);
		}

		EnemyKeyId = BlackboardComp->GetKeyID("Enemy");
		SelfKeyId = BlackboardComp->GetKeyID("SelfActor");
		AttackModeKeyId = BlackboardComp->GetKeyID("AttackModeKey");

		BlackboardComp->SetValue<UBlackboardKeyType_Object>(SelfKeyId, InPawn);

		BehaviorTreeComp->StartTree(*(MyPawn->BehaviorTree));
	}
}

void ATDAIController::OnPawnDie()
{
	UnPossess();
	MyPawn = nullptr;

	BehaviorTreeComp->StopTree(EBTStopMode::Safe);
}

AActor* ATDAIController::GetEnemy()
{
	if (MyEnemy.IsValid())
	{
		return MyEnemy.Get();
	}
	return nullptr;
}

void ATDAIController::SetEnemy(AActor* NewEnemy)
{
	AActor* Enemy = NewEnemy;

	if (MyPawn.IsValid() && NewEnemy == MyPawn.Get())
		Enemy = nullptr;

	BlackboardComp->SetValue<UBlackboardKeyType_Object>(EnemyKeyId, Enemy);
	MyEnemy = Enemy;

	if (!Enemy)
	{
		BlackboardComp->ClearValue(EnemyKeyId);
	}

}

FGenericTeamId ATDAIController::GetGenericTeamId() const
{
	// TODO: fix me
	return TeamIdMonster;
}

ETeamAttitude::Type ATDAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	FGenericTeamId OtherTeam = TeamIdHostileToAll;

	if (auto AIController = Cast<AAIController>(Other.GetInstigatorController()))
	{
		OtherTeam = AIController->GetGenericTeamId();
	}
	else if (auto PC = Cast<ATDPlayerController>(Other.GetInstigatorController()))
	{
		OtherTeam = PC->GetGenericTeamId();
	}

	if (OtherTeam == TeamIdFriendlyToAll || OtherTeam == GetGenericTeamId())
		return ETeamAttitude::Friendly;

	if (OtherTeam == TeamIdNeutralToAll)
		return ETeamAttitude::Neutral;

	return ETeamAttitude::Hostile;
}
