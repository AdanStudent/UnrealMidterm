// Fill out your copyright notice in the Description page of Project Settings.

#include "AgentCharacter.h"


// Sets default values
AAgentCharacter::AAgentCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAgentCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAgentCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAgentCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

