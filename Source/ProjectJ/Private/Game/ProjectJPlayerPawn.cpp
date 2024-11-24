// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ProjectJPlayerPawn.h"

#include "EngineUtils.h"
#include "SpringArmCameraActor.h"

// Sets default values
AProjectJPlayerPawn::AProjectJPlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
}

// Called when the game starts or when spawned
void AProjectJPlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	for (TActorIterator<ASpringArmCameraActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ASpringArmCameraActor* CameraActor = *ActorItr;
		if (CameraActor->GetName().Contains("MainCamera"))
		{
			MainCamera = CameraActor;
			break;
		}
	}
}

// Called every frame
void AProjectJPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AProjectJPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

