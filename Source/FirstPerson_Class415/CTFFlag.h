// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CTFFlag.generated.h"

UCLASS()
class FIRSTPERSON_CLASS415_API ACTFFlag : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACTFFlag();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CTF", meta = (DisplayPriority = "1"))
		UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CTF", meta = (DisplayPriority = "1"))
		UStaticMeshComponent* FlagMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CTF", meta = (DisplayPriority = "1"))
		int32 TeamID = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CTF", meta = (DisplayPriority = "1"))
		bool bIsHeld = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CTF", meta = (DisplayPriority = "1"))
		AActor* CurrentCarrier = nullptr;

	UFUNCTION()
	void OnFlagOverlap(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);

	void ResetFlag();

	FVector StartingLocation;
	FRotator StartingRotation;

	//drop flag logic
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CTF")
	float AutoReturnDelay = 10.0f;

	FTimerHandle AutoReturnTimer;

	void DropFlag(FVector DropLocation);

	//delay pickup of enemy flag after scoring or dropping
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CTF")
	bool bCanBePickedUp = true;

	FTimerHandle PickupCooldownTimer;

	void EnablePickup();

};
