// Fill out your copyright notice in the Description page of Project Settings.


#include "CTFAI_Controller.h"
#include "CTFAI_Char.h"
#include "CTFFlag.h"
#include "TimerManager.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"

void ACTFAI_Controller::BeginPlay()
{

	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("AI Controller Started"));

	MoveToEnemyFlag();
}

// moving to enemy flag handler
void ACTFAI_Controller::MoveToEnemyFlag()
{
	ACTFAI_Char* AIChar = Cast<ACTFAI_Char>(GetPawn());

	if (!AIChar || !AIChar->EnemyFlagTarget)
	{
		UE_LOG(LogTemp, Error, TEXT("Missing AIChar or EnemyFlagTarget"));
		return;
	}

	if (AIChar->bIsDead)
	{
		return;
	}

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	FNavLocation ProjectedLocation;

	if (!NavSystem || !NavSystem->ProjectPointToNavigation(
		AIChar->EnemyFlagTarget->GetActorLocation(),
		ProjectedLocation,
		FVector(500.f, 500.f, 500.f)
	))
	{
		UE_LOG(LogTemp, Error, TEXT("Could not find valid NavMesh location near flag spawn"));
		return;
	}

	EPathFollowingRequestResult::Type MoveResult =
		MoveToLocation(ProjectedLocation.Location, 25.0f, true, true, true);

	UE_LOG(LogTemp, Warning, TEXT("Moving to flag spawn. Result: %d"), MoveResult);
}


// moving to home base handler
void ACTFAI_Controller::MoveToHomeBase()
{
	ACTFAI_Char* AIChar = Cast<ACTFAI_Char>(GetPawn());

	if (!AIChar || !AIChar->HomeBaseTarget)
	{
		UE_LOG(LogTemp, Error, TEXT("Missing AIChar or HomeBaseTarget"));
		return;
	}

	MoveToActor(AIChar->HomeBaseTarget, 150.0f, true, true, true);

	UE_LOG(LogTemp, Warning, TEXT("AI returning home with flag"));
}



void ACTFAI_Controller::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	ACTFAI_Char* AIChar = Cast<ACTFAI_Char>(GetPawn());

	if (!AIChar)
	{
		return;
	}

	if (AIChar->bIsDead)
	{
		return;
	}

	if (!Result.IsSuccess())
	{
		return;
	}

	if (!AIChar->bHasFlag)
	{
		TArray<AActor*> AttachedActors;
		AIChar->GetAttachedActors(AttachedActors);

		bool bActuallyHasFlag = false;

		for (AActor* AttachedActor : AttachedActors)
		{
			if (AttachedActor && AttachedActor->IsA(ACTFFlag::StaticClass()))
			{
				bActuallyHasFlag = true;
				break;
			}
		}

		if (bActuallyHasFlag)
		{
			AIChar->bHasFlag = true;
			UE_LOG(LogTemp, Warning, TEXT("AI actually picked up enemy flag"));

			GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ACTFAI_Controller::MoveToHomeBase);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AI reached flag area but did not pick up flag. Trying again."));
			MoveToActor(AIChar->EnemyFlagTarget, 25.0f, true, true, true);
		}

		return;
	}

	AIChar->bHasFlag = false;
	UE_LOG(LogTemp, Warning, TEXT("AI reached home base / scored"));

	GetWorld()->GetTimerManager().SetTimer(
		RestartFlagRunTimer,
		this,
		&ACTFAI_Controller::MoveToEnemyFlag,
		0.5f,
		false
	);
}
