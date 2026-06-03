// Copyright Epic Games, Inc. All Rights Reserved.

#include "FirstPerson_Class415Character.h"
#include "FirstPerson_Class415Projectile.h"
#include "CTFSpawnPoint.h"
#include "CTFFlag.h"
#include "TP_WeaponComponent.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AFirstPerson_Class415Character

AFirstPerson_Class415Character::AFirstPerson_Class415Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	WeaponComponent = CreateDefaultSubobject<UTP_WeaponComponent>(TEXT("WeaponComponent"));
	WeaponComponent->SetupAttachment(Mesh1P, FName("GripPoint"));
	WeaponComponent->SetRelativeLocation(FVector::ZeroVector);
	WeaponComponent->SetRelativeRotation(FRotator::ZeroRotator);
	WeaponComponent->SetRelativeScale3D(FVector(1.f));

	CurrentHealth = MaxHealth;
}

void AFirstPerson_Class415Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	if (Mesh1P->DoesSocketExist(TEXT("GripPoint")))
	{
		UE_LOG(LogTemp, Warning, TEXT("GripPoint Exists"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GripPoint Missing"));
	}
	
}

void AFirstPerson_Class415Character::ApplyDamage(float DamageAmount) 
{
	if (bIsDead)
	{
		return;
	}

	CurrentHealth -= DamageAmount;

	UE_LOG(LogTemp, Warning, TEXT("Health: %f"), CurrentHealth);

	if (CurrentHealth <= 0.f)
	{
		CurrentHealth = 0.f;
		Death();
	}
}

void AFirstPerson_Class415Character::Death()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);

	for (AActor* AttachedActor : AttachedActors)
	{
		ACTFFlag* CarriedFlag = Cast<ACTFFlag>(AttachedActor);

		if (CarriedFlag)
		{
			CarriedFlag->DropFlag(GetActorLocation());
			break;
		}
	}

	// Hide and disable collision
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	UE_LOG(LogTemp, Warning, TEXT("Character Died."));

	GetWorldTimerManager().SetTimer(RespawnTimer, this, &AFirstPerson_Class415Character::Respawn, RespawnDelay, false);
}

void AFirstPerson_Class415Character::Respawn()
{
	bIsDead = false;
	CurrentHealth = MaxHealth;

	// show and enable collision
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);

	FVector RespawnLocation;
	FRotator RespawnRotation;

	FindRespawnLocation(RespawnLocation, RespawnRotation);

	UE_LOG(LogTemp, Warning, TEXT("Respawning at %s"), *RespawnLocation.ToString());

	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);

	for (AActor* AttachedActor : AttachedActors)
	{
		ACTFFlag* CarriedFlag = Cast<ACTFFlag>(AttachedActor);

		if (CarriedFlag)
		{
			CarriedFlag->DropFlag(GetActorLocation());
		}
	}

	SetActorLocation(RespawnLocation);
	SetActorRotation(RespawnRotation);

	UE_LOG(LogTemp, Warning, TEXT("Character Respawned"));
}

void AFirstPerson_Class415Character::FindRespawnLocation(FVector& OutLocation, FRotator& OutRotation)
{
	TArray<AActor*> FoundSpawns;
	TArray<ACTFSpawnPoint*> TeamSpawns;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACTFSpawnPoint::StaticClass(), FoundSpawns);

	for (AActor* SpawnActor : FoundSpawns)
	{
		ACTFSpawnPoint* SpawnPoint = Cast<ACTFSpawnPoint>(SpawnActor);

		if (SpawnPoint && SpawnPoint->TeamID == TeamID)
		{
			TeamSpawns.Add(SpawnPoint);
		}
	}

	if (TeamSpawns.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, TeamSpawns.Num() - 1);

		OutLocation = TeamSpawns[RandomIndex]->GetActorLocation();
		OutRotation = TeamSpawns[RandomIndex]->GetActorRotation();
		return;
	}

	OutLocation = GetActorLocation();
	OutRotation = GetActorRotation();

	UE_LOG(LogTemp, Warning, TEXT("No matching spawn point found for TeamID %d."), TeamID);
}

//////////////////////////////////////////////////////////////////////////// Input

void AFirstPerson_Class415Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFirstPerson_Class415Character::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFirstPerson_Class415Character::Look);

		if (WeaponComponent)
		{
			WeaponComponent->AttachWeapon(this);
		}
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}

	
}


void AFirstPerson_Class415Character::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AFirstPerson_Class415Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}