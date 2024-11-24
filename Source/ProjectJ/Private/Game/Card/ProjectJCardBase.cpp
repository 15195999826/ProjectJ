// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Card/ProjectJCardBase.h"

#include "PaperSpriteComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"

// Sets default values
AProjectJCardBase::AProjectJCardBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	Collider->SetupAttachment(SceneRoot);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collider);

	FrameSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("FrameSprite"));
	FrameSprite->SetupAttachment(Collider);

	Title = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Title"));
	Title->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AProjectJCardBase::BeginPlay()
{
	Super::BeginPlay();
	FrameSprite->SetVisibility(false);
}

// Called every frame
void AProjectJCardBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectJCardBase::OnDragStart()
{
	DragStartLocation = GetActorLocation();
	SetActorEnableCollision(false);
	// Stop DropOnGround animation
	if (DropOnGroundTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(DropOnGroundTimerHandle);
	}
}

void AProjectJCardBase::OnDrop(float InDuration)
{
	SetActorEnableCollision(true);
	GetWorld()->GetTimerManager().SetTimer(
		DropOnGroundTimerHandle,
		this,
		&AProjectJCardBase::UpdateDropOnGroundAnimation,
		0.01f,
		true
	);
	DropOnGroundStartTime = GetWorld()->GetTimeSeconds();
	DropOnGroundDuration = InDuration;
	DropOnGroundStartLocation = GetActorLocation();
}

void AProjectJCardBase::OnCancelDrag()
{
	SetActorEnableCollision(true);
	DragStartLocation.Z = 0.f;
	SetActorLocation(DragStartLocation);
	// 旋转重置
	SetActorRotation(FRotator::ZeroRotator);
}

void AProjectJCardBase::OnSpellFocus()
{
	FrameSprite->SetVisibility(true);
}

void AProjectJCardBase::OnLoseSpellFocus()
{
	FrameSprite->SetVisibility(false);
}

void AProjectJCardBase::OnDragTick(float DeltaSeconds, const FVector& ToLocation, float InBaseRotationX,float InMaxShakeDelta, float InLerpSpeed)
{
	const auto& Location = GetActorLocation();

	auto deltaRotationX =  (ToLocation.X - Location.X) ;
	auto deltaRotationZ = (ToLocation.Y - Location.Y) ;
            
	// deltaRotationX与deltaRotationZ的值都是-maxDelta到maxDelta之间的值
	deltaRotationX = FMath::Clamp(deltaRotationX, -InMaxShakeDelta, InMaxShakeDelta);
	deltaRotationZ = FMath::Clamp(deltaRotationZ, -InMaxShakeDelta, InMaxShakeDelta);

	FRotator baseRotation(InBaseRotationX - deltaRotationX, 0.f, deltaRotationZ);
	FQuat toRotation = FQuat(baseRotation);
	SetActorLocation(FMath::Lerp(Location, ToLocation, DeltaSeconds * InLerpSpeed));
	SetActorRotation(FQuat::Slerp(GetActorQuat(), toRotation, DeltaSeconds * InLerpSpeed));
}

// Z值从当前位置平滑下降到0
// 旋转值从当前数值平滑到0
void AProjectJCardBase::UpdateDropOnGroundAnimation()
{
	float ElapsedTime = GetWorld()->GetTimeSeconds() - DropOnGroundStartTime;
	if (ElapsedTime > DropOnGroundDuration)
	{
		// Stop the timer when the animation is complete
		GetWorld()->GetTimerManager().ClearTimer(DropOnGroundTimerHandle);
		return;
	}
	
	float Alpha = ElapsedTime / DropOnGroundDuration;
	FVector NewLocation = DropOnGroundStartLocation;
	NewLocation.Z = FMath::Lerp(DropOnGroundStartLocation.Z, 0.0f, Alpha);
	
	FRotator CurrentRotation = GetActorRotation();
	FRotator NewRotation = FMath::Lerp(CurrentRotation, FRotator::ZeroRotator, Alpha);

	SetActorLocation(NewLocation);
	SetActorRotation(NewRotation);
}

