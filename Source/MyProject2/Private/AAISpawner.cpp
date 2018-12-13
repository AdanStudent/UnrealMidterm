// Fill out your copyright notice in the Description page of Project Settings.

#include "AAISpawner.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"


// Sets default values
AAAISpawner::AAAISpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AAAISpawner::SpawnAIActor()
{
	
	if (BP_AgentCharacter)
	{
		
		FActorSpawnParameters SpawnParams;

		//Actual Spawn. The following function returns a reference to the spawned actor
		AAgentCharacter* ActorRef = GetWorld()->SpawnActor<AAgentCharacter>(BP_AgentCharacter, GetTransform(), SpawnParams);

		GLog->Log("Spawned the AIActor.");
	}
}

// Called when the game starts or when spawned
void AAAISpawner::BeginPlay()
{
	Super::BeginPlay();

}

float countdownTimer = 0;
// Called every frame
void AAAISpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FTimerHandle OutHandle;

	if (countdownTimer <= 0)
	{
		if (counter < HowManyToSpawn)
		{
			//Will call SpawnUsefulActor after the specified time
			GetWorld()->GetTimerManager().SetTimer(OutHandle, this, &AAAISpawner::SpawnAIActor, TimeToSpawn);
			counter++;
		}
		countdownTimer = TimeToSpawn;
	}
	else
	{
		countdownTimer -= DeltaTime;
	}

}

