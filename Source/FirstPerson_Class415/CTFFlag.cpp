// Fill out your copyright notice in the Description page of Project Settings.


#include "CTFFlag.h"
#include "FirstPerson_Class415Character.h"
#include "Components/BoxComponent.h"

// Sets default values
ACTFFlag::ACTFFlag()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;

	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Overlap);
	CollisionBox->SetGenerateOverlapEvents(true);

	FlagMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlagMesh"));
	FlagMesh->SetupAttachment(RootComponent);

	FlagMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ACTFFlag::BeginPlay()
{
	Super::BeginPlay();

	StartingLocation = GetActorLocation();
	StartingRotation = GetActorRotation();

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ACTFFlag::OnFlagOverlap);
}

void ACTFFlag::OnFlagOverlap(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult)
{

	// verification of being held
	if (!OtherActor || OtherActor == this || bIsHeld)
	{
		return;
	}

	// Can you pick up this flag? Death, Team, etc.
	AFirstPerson_Class415Character* Character = Cast<AFirstPerson_Class415Character>(OtherActor);

	if (!Character)
	{
		return;
	}

	if (Character->bIsDead)
	{
		return;
	}

	if (Character->TeamID == TeamID)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot Pick up your own flag."));
		return;
	}

	//attachs flag and ensures not duplicate flags are made to be carried.
	TArray<AActor*> AttachedActors;
	OtherActor->GetAttachedActors(AttachedActors);

	for (AActor* AttachedActor : AttachedActors)
	{
		if (AttachedActor && AttachedActor->IsA(ACTFFlag::StaticClass()))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s is already carrying a flag."), *OtherActor->GetName());
			return;
		}
	}

	GetWorldTimerManager().ClearTimer(AutoReturnTimer);

	bIsHeld = true;
	CurrentCarrier = OtherActor;

	AttachToActor(OtherActor, FAttachmentTransformRules::KeepWorldTransform);

	UE_LOG(LogTemp, Warning, TEXT("Flag Picked up by: %s"), *OtherActor->GetName());
}

void ACTFFlag::ResetFlag()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	bIsHeld = false;
	CurrentCarrier = nullptr;

	SetActorLocation(StartingLocation);
	SetActorRotation(StartingRotation);

	UE_LOG(LogTemp, Warning, TEXT("Flag reset."));
}

void ACTFFlag::DropFlag(FVector DropLocation)
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	bIsHeld = false;
	CurrentCarrier = nullptr;

	SetActorLocation(DropLocation);

	GetWorldTimerManager().SetTimer(AutoReturnTimer,this,&ACTFFlag::ResetFlag,AutoReturnDelay,false);

	UE_LOG(LogTemp, Warning, TEXT("Flag dropped. Returning in %f seconds."), AutoReturnDelay);
}

// Called every frame
void ACTFFlag::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

