	// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CTFAI_Controller.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSON_CLASS415_API ACTFAI_Controller : public AAIController
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	void MoveToEnemyFlag();
	void MoveToHomeBase();

	FTimerHandle RestartFlagRunTimer;

protected:

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	
	
};
