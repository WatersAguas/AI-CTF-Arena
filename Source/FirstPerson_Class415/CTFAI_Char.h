// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FirstPerson_Class415Character.h"
#include "CTFAI_Char.generated.h"

/**
 * 
 */
class AFirstPerson_Class415Projectile;

UCLASS()
class FIRSTPERSON_CLASS415_API ACTFAI_Char : public AFirstPerson_Class415Character
{
	GENERATED_BODY()

public:

	virtual void Respawn() override;

	ACTFAI_Char();

	// Flag control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CTF")
	bool bHasFlag = false;


	// AI movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AttackRange = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	AActor* MoveTarget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	AActor* EnemyFlagTarget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	AActor* HomeBaseTarget = nullptr;


	// AI Combat
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat")
	TSubclassOf<AFirstPerson_Class415Projectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat")
	float FireRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat")
	float ProjectileSpawnDistance = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat")
	float ProjectileSpawnHeight = 60.f;

	bool bCanShoot = true;

	FTimerHandle FireRateTimer;

	// AI shooting Functions
	virtual void Tick(float DeltaTime) override;

	void TryShootEnemy();
	AFirstPerson_Class415Character* FindClosestEnemy();
	void ShootAtTarget(AActor* Target);
	void ResetCanShoot();

	//smarter AI combat to go for the kill
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat")
	float CombatStopRange = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat")
	float ResumeObjectiveDelay = 1.0f;

	bool bIsInCombat = false;

	FTimerHandle ResumeObjectiveTimer;

	void EnterCombat(AFirstPerson_Class415Character* TargetEnemy);
	void ExitCombat();
	
	// Strafing during Combat
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat")
	float StrafeDistance = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat")
	float StrafeInterval = 0.8f;

	FTimerHandle CombatMoveTimer;

	void CombatStrafe();

};
