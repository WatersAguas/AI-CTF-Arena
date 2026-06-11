// Fill out your copyright notice in the Description page of Project Settings.

#include "CTFAI_Char.h"
#include "CTFAI_Controller.h"
#include "FirstPerson_Class415Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void ACTFAI_Char::Respawn()
{
	Super::Respawn();

	bHasFlag = false;

	ACTFAI_Controller* AIController = Cast<ACTFAI_Controller>(GetController());

	if (AIController)
	{
		AIController->StopMovement();
		AIController->MoveToEnemyFlag();
	}
}
// ticks to try and shoot at player
void ACTFAI_Char::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TryShootEnemy();
}

//will attempt to shoot at player
void ACTFAI_Char::TryShootEnemy()
{
	if (bIsDead || !bCanShoot)
	{
		return;
	}

	AFirstPerson_Class415Character* TargetEnemy = FindClosestEnemy();

	if (!TargetEnemy)
	{
		return;
	}

	float DistanceToEnemy = FVector::Dist(GetActorLocation(), TargetEnemy->GetActorLocation());

	if (DistanceToEnemy <= AttackRange)
	{
		EnterCombat(TargetEnemy);
		ShootAtTarget(TargetEnemy);
	}
	else if (bIsInCombat)
	{
		ExitCombat();
	}
}

// Find closest enemy to shoot at
AFirstPerson_Class415Character* ACTFAI_Char::FindClosestEnemy()
{
	TArray<AActor*> FoundCharacters;

	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		AFirstPerson_Class415Character::StaticClass(),
		FoundCharacters
	);

	AFirstPerson_Class415Character* ClosestEnemy = nullptr;
	float ClosestDistance = FLT_MAX;

	for (AActor* Actor : FoundCharacters)
	{
		AFirstPerson_Class415Character* Character = Cast<AFirstPerson_Class415Character>(Actor);

		if (!Character)
		{
			continue;
		}

		if (Character == this)
		{
			continue;
		}

		if (Character->bIsDead)
		{
			continue;
		}

		if (Character->TeamID == TeamID)
		{
			continue;
		}

		float Distance = FVector::Dist(GetActorLocation(), Character->GetActorLocation());

		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestEnemy = Character;
		}
	}

	return ClosestEnemy;
}

//shoots at target
void ACTFAI_Char::ShootAtTarget(AActor* Target)
{
	if (!ProjectileClass || !Target)
	{
		return;
	}

	FVector TargetLocation = Target->GetActorLocation() + FVector(0.f, 0.f, 50.f);

	FVector Direction = TargetLocation - GetActorLocation();
	Direction.Normalize();

	FVector SpawnLocation = GetActorLocation() + Direction * ProjectileSpawnDistance + FVector(0.f, 0.f, ProjectileSpawnHeight);

	FRotator SpawnRotation = Direction.Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	AFirstPerson_Class415Projectile* Projectile = GetWorld()->SpawnActor<AFirstPerson_Class415Projectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (Projectile)
	{
		Projectile->ProjectileOwner = this;
		Projectile->OwnerTeamID = TeamID;

		UE_LOG(LogTemp, Warning, TEXT("AI fired projectile. AI TeamID: %d"), TeamID);
	}

	bCanShoot = false;

	GetWorldTimerManager().SetTimer(FireRateTimer, this, &ACTFAI_Char::ResetCanShoot, FireRate, false);
}

// allows AI to shoot after the delay 
void ACTFAI_Char::ResetCanShoot()
{
	bCanShoot = true;
}

void ACTFAI_Char::EnterCombat(AFirstPerson_Class415Character* TargetEnemy)
{
	if (!TargetEnemy)
	{
		return;
	}

	bIsInCombat = true;

	ACTFAI_Controller* AIController = Cast<ACTFAI_Controller>(GetController());

	if (AIController)
	{
		AIController->StopMovement();
	}

	FVector Direction = TargetEnemy->GetActorLocation() - GetActorLocation();
	Direction.Z = 0.f;

	if (!Direction.IsNearlyZero())
	{
		SetActorRotation(Direction.Rotation());
	}

	GetWorldTimerManager().ClearTimer(ResumeObjectiveTimer);

	GetWorldTimerManager().SetTimer(ResumeObjectiveTimer, this, &ACTFAI_Char::ExitCombat, ResumeObjectiveDelay, false);

	if (!GetWorldTimerManager().IsTimerActive(CombatMoveTimer))
	{
		GetWorldTimerManager().SetTimer(CombatMoveTimer, this, &ACTFAI_Char::CombatStrafe, StrafeInterval, true);
	}
}

void ACTFAI_Char::ExitCombat()
{
	GetWorldTimerManager().ClearTimer(CombatMoveTimer);

	bIsInCombat = false;

	if (bIsDead)
	{
		return;
	}


	ACTFAI_Controller* AIController = Cast<ACTFAI_Controller>(GetController());

	if (!AIController)
	{
		return;
	}

	if (bHasFlag)
	{
		AIController->MoveToHomeBase();
	}
	else
	{
		AIController->MoveToEnemyFlag();
	}
}

void ACTFAI_Char::CombatStrafe()
{
	if (bIsDead || !bIsInCombat)
	{
		return;
	}

	ACTFAI_Controller* AIController = Cast<ACTFAI_Controller>(GetController());

	if (!AIController)
	{
		return;
	}

	float Direction = FMath::RandBool() ? 1.f : -1.f;

	FVector StrafeTarget = GetActorLocation() + (GetActorRightVector() * Direction * StrafeDistance);

	AIController->MoveToLocation(StrafeTarget, 50.f, true, true, true);
}

ACTFAI_Char::ACTFAI_Char()
{
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = ACTFAI_Controller::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}





