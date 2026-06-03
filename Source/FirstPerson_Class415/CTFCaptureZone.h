// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "CTFCaptureZone.generated.h"

UCLASS()
class FIRSTPERSON_CLASS415_API ACTFCaptureZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACTFCaptureZone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CTF Zone", meta = (DisplayPriority = "1"))
		UBoxComponent* CollisionBox;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "CTF Zone", meta = (DisplayPriority = "1"))
		int32 TeamID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CTF Zone", meta = (DisplayPriority = "1"))
		int32 RedScore = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CTF Zone", meta = (DisplayPriority = "1"))
		int32 BlueScore = 0;

	UFUNCTION()
		void OnZoneOverlap(
			UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult
		);

};
