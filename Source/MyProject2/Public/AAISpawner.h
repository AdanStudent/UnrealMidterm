// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AgentCharacter.h"
#include "GameFramework/Actor.h"
#include "AAISpawner.generated.h"

UCLASS()
class MYPROJECT2_API AAAISpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAAISpawner();

	UPROPERTY(EditDefaultsOnly, Category = "ActorSpawning")
		TSubclassOf<AAgentCharacter> BP_AgentCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "ActorSpawning")
		float TimeToSpawn = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "ActorSpawning")
		int HowManyToSpawn = 10;

	UFUNCTION()
		void SpawnAIActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
		int counter = 0;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
