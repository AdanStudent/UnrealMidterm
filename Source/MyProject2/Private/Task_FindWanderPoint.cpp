// Fill out your copyright notice in the Description page of Project Settings.

#include "Task_FindWanderPoint.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BehaviorTreeComponent.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "AgentAIController.h"
#include "AIWanderPoint.h"
#include "Engine.h"

static FName Name_WanderPointKey("WanderPoint");
EBTNodeResult::Type UTask_FindWanderPoint::ExecuteTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAgentAIController* AIController = Cast<AAgentAIController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* AIBlackboard = AIController->GetBlackboardComp();

	if (!AIController || !AIBlackboard)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("This is an on screen message!"));
		return EBTNodeResult::Failed;
	}

	TArray<AActor*> WanderPoints;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAIWanderPoint::StaticClass(), WanderPoints);

	if (WanderPoints.Num() == 0) 
	{
		return EBTNodeResult::Failed;
	}

	AAIWanderPoint* CurrentWanderPoint = Cast<AAIWanderPoint>(AIBlackboard->GetValueAsObject(Name_WanderPointKey));
	AAIWanderPoint* NewWanderPoint = Cast<AAIWanderPoint>(WanderPoints[FMath::RandRange(0, WanderPoints.Num() - 1)]);

	//will break if only 1 WanderPoint
	while (NewWanderPoint == CurrentWanderPoint)
	{
		NewWanderPoint = Cast<AAIWanderPoint>(WanderPoints[FMath::RandRange(0, WanderPoints.Num() - 1)]);
	}

	AIBlackboard->SetValueAsObject(Name_WanderPointKey, NewWanderPoint);

	return EBTNodeResult::Succeeded;
}
