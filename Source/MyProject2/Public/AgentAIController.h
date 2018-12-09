// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AgentAIController.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT2_API AAgentAIController : public AAIController
{
	GENERATED_BODY()
	
protected:
	AAgentAIController();

	virtual void Possess(APawn* InPawn) override;

	virtual bool HasLineOfSightTo(const AActor* Other) const;

	UPROPERTY(EditDefaultsOnly)
	class UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(EditDefaultsOnly)
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditDefaultsOnly)
	class UBehaviorTreeComponent* BehaviorTreeComp;

	UPROPERTY(EditDefaultsOnly)
	class UBlackboardComponent* BlackboardComp;

	UFUNCTION()
	void OnSeePawn(class APawn* SensedPawn);

public:
	FORCEINLINE class UBlackboardComponent* GetBlackboardComp() const { return BlackboardComp; }
	
	
};
