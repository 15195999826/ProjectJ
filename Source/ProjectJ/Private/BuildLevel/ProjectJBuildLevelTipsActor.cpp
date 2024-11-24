// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildLevel/ProjectJBuildLevelTipsActor.h"

#include "Components/TextRenderComponent.h"

// Sets default values
AProjectJBuildLevelTipsActor::AProjectJBuildLevelTipsActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;
	
	TextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponent"));
	TextRenderComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AProjectJBuildLevelTipsActor::BeginPlay()
{
	Super::BeginPlay();
	SetActorEnableCollision(false);
}

