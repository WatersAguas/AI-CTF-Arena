// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FirstPerson_Class415Character.h"
#include "CTFAI_Char.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSON_CLASS415_API ACTFAI_Char : public AFirstPerson_Class415Character
{
	GENERATED_BODY()

public:

	virtual void Respawn() override;

	ACTFAI_Char();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CTF")
	bool bHasFlag = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AttackRange = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	AActor* MoveTarget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	AActor* EnemyFlagTarget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	AActor* HomeBaseTarget = nullptr;



	
};
