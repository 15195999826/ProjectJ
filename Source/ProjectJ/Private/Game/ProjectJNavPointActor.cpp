// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ProjectJNavPointActor.h"

#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "Core/System/ProjectJContextSystem.h"
#include "Game/ProjectJGameProgress.h"

// Sets default values
AProjectJNavPointActor::AProjectJNavPointActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(SceneRoot);

	Title = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Title"));
	Title->SetupAttachment(SceneRoot);
}

// Called when the game starts or when spawned
void AProjectJNavPointActor::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectJNavPointActor::OnDragStart()
{
	if (!CanDrag)
	{
		return;
	}

	DragStartLocation = GetActorLocation();
}

void AProjectJNavPointActor::OnDragTick(float DeltaSeconds, const FVector& ToLocation, float InBaseRotationX,
	float InMaxShakeDelta, float InLerpSpeed)
{
	if (!CanDrag)
	{
		return;
	}

	SetActorLocation(ToLocation);
}

void AProjectJNavPointActor::OnDrop(float InDropDuration)
{
	if (!CanDrag)
	{
		return;
	}

	// 设置当前位置Z为0
	FVector Location = GetActorLocation();
	Location.Z = 0;
	SetActorLocation(Location);
}

void AProjectJNavPointActor::OnCancelDrag()
{
	if (!CanDrag)
	{
		return;
	}

	SetActorLocation(DragStartLocation);
}

void AProjectJNavPointActor::BindConfig(const FProjectJNavPoint& InConfig)
{
	CanDrag = false;
	ToLevel = InConfig.ToLevel;
	Title->SetText(FText::FromString(InConfig.ToLevel.ToString()));
	SetActorLocation(InConfig.Location);
	OnClicked.Clear();
	OnClicked.AddDynamic(this, &AProjectJNavPointActor::OnNavPointClicked);
}

void AProjectJNavPointActor::OnNavPointClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	// Todo: 跳转到指定关卡
	auto GameProgress = GetWorld()->GetSubsystem<UProjectJContextSystem>()->GameProgress;
	GameProgress->EnterLevel(ToLevel);
}


