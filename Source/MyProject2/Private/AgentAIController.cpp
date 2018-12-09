// Fill out your copyright notice in the Description page of Project Settings.

#include "AgentAIController.h"
#include "Runtime/AIModule/Classes/Perception/PawnSensingComponent.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BehaviorTreeComponent.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "AgentCharacter.h"
#include "Engine.h"


static FName Name_ShouldWanderKey("ShouldWander");
static FName Name_EnemyKey("Enemy");

AAgentAIController::AAgentAIController()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComponent");

	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTreeComponent");

	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>("BlackboardComponent");

}


void AAgentAIController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);

	PawnSensingComp->OnSeePawn.AddDynamic(this, &AAgentAIController::OnSeePawn);

	if (BehaviorTree)
	{
		BlackboardComp->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
		BehaviorTreeComp->StartTree(*BehaviorTree);

		BlackboardComp->SetValueAsBool(Name_ShouldWanderKey, true);
	}
}

bool AAgentAIController::HasLineOfSightTo(const AActor * Other) const
{

	if (BlackboardComp && Other)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("I don't see you"));
		BlackboardComp->SetValueAsBool(Name_ShouldWanderKey, true);
		BlackboardComp->SetValueAsObject(Name_EnemyKey, NULL);
		return true;
	}
	
	return false;
}

void AAgentAIController::OnSeePawn(APawn * SensedPawn)
{
	if (BlackboardComp && SensedPawn) 
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("I see you"));

		BlackboardComp->SetValueAsBool(Name_ShouldWanderKey, false);
		BlackboardComp->SetValueAsObject(Name_EnemyKey, SensedPawn);

	}
}
