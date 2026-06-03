// Copyright Epic Games, Inc. All Rights Reserved.

#include "FirstPerson_Class415Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "PerlinProcTerrain.h"
#include "FirstPerson_Class415Character.h"



AFirstPerson_Class415Projectile::AFirstPerson_Class415Projectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AFirstPerson_Class415Projectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	ballMesh = CreateDefaultSubobject<UStaticMeshComponent>("Ball Mesh");

	// Set as root component
	RootComponent = CollisionComp;

	ballMesh->SetupAttachment(CollisionComp);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

// created begin play for projectile and when it enters the game.
void AFirstPerson_Class415Projectile::BeginPlay()
{
	Super::BeginPlay();
	// sets random color generator so it can be applied later.
	randColor = FLinearColor(UKismetMathLibrary::RandomFloatInRange(0.f, 1.f), UKismetMathLibrary::RandomFloatInRange(0.f, 1.f), UKismetMathLibrary::RandomFloatInRange(0.f, 1.f), 1.f);

	// place holder is defined to then create and plug into the projMat and then apply to the ball/projectile.
	dmiMat = UMaterialInstanceDynamic::Create(projMat, this);
	ballMesh->SetMaterial(0, dmiMat);

	// allows the place holder to also change teh color accordingly to the random color generator.
	dmiMat->SetVectorParameterValue("projColor", randColor);
}

void AFirstPerson_Class415Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	if (!OtherActor || OtherActor == this || OtherActor == ProjectileOwner)
	{
		return;
	}

	if (HandleCharacterHit(OtherActor))
	{
		Destroy();
		return;
	}

	if (HandlePhysicsHit(OtherComp))
	{
		Destroy();
		return;
	}

	SpawnHitEffects(HitComp, OtherActor, Hit);

	Destroy();

}

bool AFirstPerson_Class415Projectile::HandleCharacterHit(AActor * OtherActor)
{
	AFirstPerson_Class415Character* HitCharacter = Cast< AFirstPerson_Class415Character>(OtherActor);

	if (!HitCharacter)
	{
		return false;
	}

	if (HitCharacter->TeamID == OwnerTeamID)
	{
		UE_LOG(LogTemp, Warning, TEXT("Friendly Fire Ignored"));
		return true;
	}

	HitCharacter->ApplyDamage(DamageAmount);
	return true;
}

bool AFirstPerson_Class415Projectile::HandlePhysicsHit(UPrimitiveComponent * OtherComp)
{
	if (!OtherComp || !OtherComp->IsSimulatingPhysics())
	{
		return false;
	}

	OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
	return true;

}

void AFirstPerson_Class415Projectile::SpawnHitEffects(UPrimitiveComponent * HitComp, AActor * OtherActor, const FHitResult & Hit)
{
	if (colorP)
	{
		UNiagaraComponent* particleComp = UNiagaraFunctionLibrary::SpawnSystemAttached(colorP, HitComp, NAME_None, FVector(-20.f, 0.f, 0.f), FRotator(0.f), EAttachLocation::KeepRelativeOffset, true);

		if (particleComp)
		{
			particleComp->SetNiagaraVariableLinearColor(FString("RandomColor"), randColor);
		}

		if (ballMesh)
		{
			ballMesh->DestroyComponent();
		}

		if (CollisionComp)
		{
			CollisionComp->BodyInstance.SetCollisionProfileName("NoCollision");
		}
	}

	if (baseMat)
	{
		float frameNum = UKismetMathLibrary::RandomFloatInRange(0.f, 3.f);

		UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), baseMat, FVector(UKismetMathLibrary::RandomFloatInRange(20.f, 40.f)), Hit.Location, Hit.Normal.Rotation(), 0.f);

			if (Decal)
			{
				UMaterialInstanceDynamic* MatInstance = Decal->CreateDynamicMaterialInstance();

				if (MatInstance)
				{
					MatInstance->SetVectorParameterValue("Color", randColor);
					MatInstance->SetScalarParameterValue("Frame", frameNum);
				}
			}
	}

	APerlinProcTerrain* procTerrain = Cast<APerlinProcTerrain>(OtherActor);

	if (procTerrain)
	{
		procTerrain->AlterMesh(Hit.ImpactPoint);
	}
}



	/*
	//ignore owner
	if (OtherActor == ProjectileOwner)
	{
		return;
	}

	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
		return;
	}

	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	// Apply Damage to Character
	AFirstPerson_Class415Character* HitCharacter = Cast<AFirstPerson_Class415Character>(OtherActor);

	if (HitCharacter)
	{
		if (HitCharacter->TeamID == OwnerTeamID)
		{
			UE_LOG(LogTemp, Warning, TEXT("Friendly fire Ignored"));
			Destroy();
			return;
		}

		HitCharacter->ApplyDamage(DamageAmount);
		Destroy();
		return;
	}

	if (OtherActor != nullptr)
	{
		// Niagara system added here
		if (colorP)
		{
			UNiagaraComponent* particleComp = UNiagaraFunctionLibrary::SpawnSystemAttached(colorP, HitComp, NAME_None, FVector(-20.f, 0.f, 0.f), FRotator(0.f), EAttachLocation::KeepRelativeOffset, true);
			particleComp->SetNiagaraVariableLinearColor(FString("RandomColor"), randColor);
			ballMesh->DestroyComponent();
			CollisionComp->BodyInstance.SetCollisionProfileName("NoCollision");
		}

		float frameNum = UKismetMathLibrary::RandomFloatInRange(0.f, 3.f);

		auto Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), baseMat, FVector(UKismetMathLibrary::RandomFloatInRange(20.f, 40.f)), Hit.Location, Hit.Normal.Rotation(), 0.f);
		auto MatInstance = Decal->CreateDynamicMaterialInstance();

		MatInstance->SetVectorParameterValue("Color", randColor);
		MatInstance->SetScalarParameterValue("Frame", frameNum);

		APerlinProcTerrain* procTerrain = Cast<APerlinProcTerrain>(OtherActor);

		if(procTerrain)
		{
			procTerrain->AlterMesh(Hit.ImpactPoint);
		}

	}*/