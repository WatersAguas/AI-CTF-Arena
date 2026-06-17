// Fill out your copyright notice in the Description page of Project Settings.


#include "CTFCaptureZone.h"
#include "CTFFlag.h"
#include "CTFAI_Char.h"
#include "CTFAI_Controller.h"
#include "FirstPerson_Class415Character.h"
#include "UObject/Interface.h"
#include "Components/BoxComponent.h"

// Sets default values
ACTFCaptureZone::ACTFCaptureZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;

	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Overlap);
	CollisionBox->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void ACTFCaptureZone::BeginPlay()
{
	Super::BeginPlay();

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ACTFCaptureZone::OnZoneOverlap);
}

// Called every frame
void ACTFCaptureZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACTFCaptureZone::OnZoneOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Checks Player matches ID, not dead and is a character to pickup own flag
	AFirstPerson_Class415Character* Character = Cast<AFirstPerson_Class415Character>(OtherActor);

	if (!Character)
	{
	return;
	}

	if (Character->bIsDead)
	{
		return;
	}

	if (Character->TeamID != TeamID)
	{
		UE_LOG(LogTemp, Warning, TEXT("Wrong Team for this capture zone"));
		return;
	}

	//Attachment Logic for Player
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	TArray<AActor*> AttachedActors;
	OtherActor->GetAttachedActors(AttachedActors);

	for (AActor* AttachedActor : AttachedActors)
	{
		ACTFFlag* CarriedFlag = Cast<ACTFFlag>(AttachedActor);

		if (CarriedFlag && CarriedFlag->TeamID != TeamID)
		{
			if (TeamID == 0)
			{
				RedScore++;
				UE_LOG(LogTemp, Warning, TEXT("Red Team Scored! Score: %d"), RedScore);
			}
			else if (TeamID == 1)
			{
				BlueScore++;
				UE_LOG(LogTemp, Warning, TEXT("Blue Team Scored! Score: %d"), BlueScore);
			}

			UE_LOG(LogTemp, Warning, TEXT("SENDING SCORE -> Red: %d Blue: %d"), RedScore, BlueScore);


			//UI Addition
			APlayerController* PC = GetWorld()->GetFirstPlayerController();
			AFirstPerson_Class415Character* PlayerChar = Cast<AFirstPerson_Class415Character>(PC->GetPawn());

			if (PlayerChar && PlayerChar->ScoreboardWidget)
			{
				UFunction* Func = PlayerChar->ScoreboardWidget->FindFunction(TEXT("UpdateScore"));

				if (Func)
				{
					struct
					{
						int32 RedScore;
						int32 BlueScore;
					} Params;

					Params.RedScore = RedScore;
					Params.BlueScore = BlueScore;

					PlayerChar->ScoreboardWidget->ProcessEvent(Func, &Params);
				}
			}

			CarriedFlag->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			CarriedFlag->ResetFlag();

			ACTFAI_Char* AIChar = Cast<ACTFAI_Char>(Character);

			if (AIChar)
			{
				AIChar->bHasFlag = false;

				ACTFAI_Controller* AIController = Cast<ACTFAI_Controller>(AIChar->GetController());

				if (AIController)
				{
					AIController->StopMovement();

					GetWorld()->GetTimerManager().SetTimerForNextTick(
						AIController,
						&ACTFAI_Controller::MoveToEnemyFlag
					);
				}
			}

			return;
		}
	}

}	

