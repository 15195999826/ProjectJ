// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ProjectJGameContext.h"

// Sets default values
AProjectJGameContext::AProjectJGameContext()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AProjectJGameContext::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectJGameContext::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

