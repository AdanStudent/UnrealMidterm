// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Animation/AnimInstance.h"


APlayerCharacter::APlayerCharacter() 
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));

	if (SpringArm)
	{
		SpringArm->SetupAttachment(GetCapsuleComponent());
		SpringArm->RelativeLocation = FVector(-20.0f, 0.0f, 40.0f);
		SpringArm->TargetArmLength = 0.0f;
		SpringArm->bDoCollisionTest = false;
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->bInheritPitch = false;
		SpringArm->bInheritRoll = true;
		SpringArm->bInheritYaw = true;
	}

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	if (Camera)
	{
		Camera->FieldOfView = 90.0f;
		Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	}

	FP_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Mesh"));
	if (FP_Mesh)
	{
		FP_Mesh->SetupAttachment(Camera);
	}

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	if (GunMesh)
	{
		GunMesh->SetupAttachment(Camera);
	}

	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
	
	bCanShoot = true;
	bUnlimitedAmmo = false;
	MaxAmmo = 20;
	CurrentAmmo = MaxAmmo;

	BaseTurnRate = 100.0f;
	BaseLookUpRate = 100.0f;
	CameraPitchMin = -89.0f;
	CameraPitchMax = 89.0f;

	SprintSpeed = 1500.0f;

	DamageAmount = 10.0f;

	FireAnimation = nullptr;

	TrailEffect = nullptr;
	HitEffect = nullptr;

}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// This will attach the gun to the hand's mesh
	GunMesh->AttachToComponent(FP_Mesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("GripPoint"));

	if (CurrentHealth != MaxHealth)
	{
		CurrentHealth = MaxHealth;
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Assertion check
	check(PlayerInputComponent);

	//Shoot Input
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerCharacter::OnFire);

	// Movement input
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::OnSprintStart);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::OnSprintEnd);

	// Camera input
	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUpRate);
}

void APlayerCharacter::OnDeath_Implementation()
{
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//Don't continue if health is already below or at zero
	if (CurrentHealth <= 0.0f)
	{
		return 0.0f;
	}

	const float SubtractedHealth = CurrentHealth - DamageAmount;

	if (FMath::IsNearlyZero(SubtractedHealth) || SubtractedHealth < 0.0f)
	{
		CurrentHealth = 0.0f;
		OnDeath();
	}
	else
	{
		CurrentHealth = SubtractedHealth;
	}

	return CurrentHealth;
}

void APlayerCharacter::OnFire_Implementation()
{
	if (bCanShoot)
	{
		if (!HasAmmo() && !bUnlimitedAmmo)
		{
			return;
		}
	}
	else
	{
		return;
	}
	//declare our particle's ending location to be set in the line trace
	FVector ParticleLocation = FVector::ZeroVector;

	//prepare our invisible ray's values
	FHitResult Hit;
	const FVector StartTrace = Camera->GetComponentLocation();
	const FVector EndTrace = StartTrace + (Camera->GetForwardVector() * 10000.0f);

	//collision objects to ignore
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.AddIgnoredActor(this);

	//fire an invisible ray
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECollisionChannel::ECC_Visibility, QueryParams);

	//if we hit something then damage it and set out particle location
	if (Hit.bBlockingHit && Hit.GetActor()) 
	{
		//set the location for the particles to end at
		ParticleLocation = Hit.ImpactPoint;

		//create the object for a general object type
		TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(
			UDamageType::StaticClass());

		//set out general damage type to a specific type: Point Damage
		FPointDamageEvent PointDamageEvent(DamageAmount, Hit, Hit.ImpactNormal, ValidDamageTypeClass);

		//Damage Actor
		Hit.GetActor()->TakeDamage(DamageAmount, PointDamageEvent, GetController(), GetOwner());
	}
	else
	{
		//set the location for the particles to end at 
		ParticleLocation = EndTrace;
	}

	SpawnShootingParticles(ParticleLocation);

	//play the Fire animation
	if (FireAnimation)
	{
		if (UAnimInstance* AnimInstance = FP_Mesh->GetAnimInstance()) 
		{
			AnimInstance->Montage_Play(FireAnimation);
		}
	}

	//if we dont have unlimited ammon on, then use up 1 bullet
	if (!bUnlimitedAmmo)
	{
		CurrentAmmo--;
	}
}

void APlayerCharacter::OnSprintStart_Implementation()
{
	// Stop us from being able to shoot and turn on the flag for sprinting
	bIsSprinting = true;
	bCanShoot = false;
	// Save what our walk speed was initially
	PreviousWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	// Update our current walk speed to the sprint speed
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void APlayerCharacter::OnSprintEnd_Implementation()
{
	// Turn off the flag for sprinting
	bIsSprinting = false;
	// Revert our walk speed from sprint to regular walking speed
	GetCharacterMovement()->MaxWalkSpeed = PreviousWalkSpeed;
	// Allow us to shoot again
	bCanShoot = true;
}

void APlayerCharacter::MoveForward(float Scalar)
{
	if (Scalar != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Scalar);
	}
}

void APlayerCharacter::MoveRight(float Scalar)
{
	if (Scalar != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Scalar);
	}
}

void APlayerCharacter::LookUpRate(float Rate)
{
	// Only continue if the spring arm is valid
	if (SpringArm)
	{
		// Get the current rotation
		FRotator CameraRelativeRot = SpringArm->RelativeRotation;
		// Get the intended pitch
		const float CameraNewPitch = FMath::ClampAngle(CameraRelativeRot.Pitch + Rate * BaseLookUpRate * GetWorld()->DeltaTimeSeconds,
			CameraPitchMin, CameraPitchMax);

		// Combine the values to become the new intended pitch
		CameraRelativeRot.Pitch = CameraNewPitch;

		// Confirm/Update the spring arm rotation
		SpringArm->SetRelativeRotation(CameraRelativeRot);
	}
}

void APlayerCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->DeltaTimeSeconds);
}

void APlayerCharacter::SpawnShootingParticles(FVector ParticleLocation)
{
	// Check if our trail effect pointer is valid
	if (TrailEffect)
	{
		// Spawn the particle
		UParticleSystemComponent* SpawnedParticle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), TrailEffect, GunMesh->GetSocketLocation(FName("Muzzle")));

		// Scale the particle up so its easily visible
		SpawnedParticle->SetWorldScale3D(FVector(5.0f));

		// Set the end of the particle beam
		SpawnedParticle->SetVectorParameter(FName("ShockBeamEnd"), ParticleLocation);
	}

	// Check if our hit effect pointer is valid
	if (HitEffect)
	{
		// Spawn the particle
		UParticleSystemComponent* SpawnedParticle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), HitEffect, ParticleLocation, FRotator::ZeroRotator, true);

		// Scale the particle up so its easily visible
		SpawnedParticle->SetWorldScale3D(FVector(0.25f));
	}
}


