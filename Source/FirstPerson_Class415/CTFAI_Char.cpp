// Fill out your copyright notice in the Description page of Project Settings.

#include "CTFAI_Char.h"
#include "CTFAI_Controller.h"

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

ACTFAI_Char::ACTFAI_Char()
{
	AIControllerClass = ACTFAI_Controller::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}



