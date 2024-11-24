// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ProjectJSceneUIManager.h"

#include "Components/WidgetComponent.h"
#include "Core/System/ProjectJContextSystem.h"
#include "Game/ProjectJLevelSettingActor.h"

// Sets default values
AProjectJSceneUIManager::AProjectJSceneUIManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	TalkWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("TalkWidgetComponent"));
	TalkWidgetComponent->SetupAttachment(SceneRoot);
}

// Called when the game starts or when spawned
void AProjectJSceneUIManager::BeginPlay()
{
	Super::BeginPlay();
	TalkWidgetComponent->SetVisibility(false);
}

// Called every frame
void AProjectJSceneUIManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectJSceneUIManager::StartTalkWith_Implementation(AProjectJCardBase* InCard)
{
	auto LevelSetting = GetWorld()->GetSubsystem<UProjectJContextSystem>()->LevelSettingActor;
	SetActorLocation(InCard->GetActorLocation() + LevelSetting->TalkWidgetOffset);
	TalkWidgetComponent->SetVisibility(true);
}


