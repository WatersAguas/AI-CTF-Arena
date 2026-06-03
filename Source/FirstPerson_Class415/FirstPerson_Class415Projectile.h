// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInterface.h"
#include "FirstPerson_Class415Projectile.generated.h"


class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;

UCLASS(config=Game)
class AFirstPerson_Class415Projectile : public AActor
{
	GENERATED_BODY()


public:
	AFirstPerson_Class415Projectile();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	AActor* ProjectileOwner = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 OwnerTeamID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float DamageAmount = 25.f;

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }


protected:
	virtual void BeginPlay() override;

	bool HandleCharacterHit(AActor* OtherActor);
	bool HandlePhysicsHit(UPrimitiveComponent* OtherComp);
	void SpawnHitEffects(UPrimitiveComponent* HitComp, AActor* OtherActor, const FHitResult& Hit);

private:
	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	// used to tie in the ballMesh with the code and colors.
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ballMesh;

	// used for base mat for the splash.
	UPROPERTY(EditAnywhere)
	UMaterialInterface* baseMat;

	// random color generator for the splat and projectile.
	UPROPERTY()
	FLinearColor randColor;

	// used to add the random color to teh projectile.
	UPROPERTY(EditAnywhere)
	UMaterialInterface* projMat;

	// place holder to apply to projectile
	UPROPERTY()
	UMaterialInstanceDynamic* dmiMat;

	// Place holder for the color of the Niagara
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* colorP;
};

