// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Card/ProjectJCardBase.h"

#include "PaperSpriteComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "Core/DeveloperSettings/ProjectJGeneralSettings.h"
#include "Core/System/ProjectJContextSystem.h"

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
	Title->SetupAttachment(Mesh, TEXT("立方体"));
}

// Called when the game starts or when spawned
void AProjectJCardBase::BeginPlay()
{
	Super::BeginPlay();
	FrameSprite->SetVisibility(false);
}

void AProjectJCardBase::PrintMeshSize()
{
	const auto Bound = Mesh->Bounds;
	auto LeftTop = Bound.Origin + FVector(-Bound.BoxExtent.X, Bound.BoxExtent.Y, Bound.BoxExtent.Z);
	auto RightTop = Bound.Origin + FVector(Bound.BoxExtent.X, Bound.BoxExtent.Y, Bound.BoxExtent.Z);
	auto RightBottom = Bound.Origin + FVector(Bound.BoxExtent.X, -Bound.BoxExtent.Y, Bound.BoxExtent.Z);
	// 在bound的中心和左右上角绘制DebugSphere
	DrawDebugSphere(GetWorld(), Bound.Origin, 10.f, 12, FColor::Red, false, 5.f);
	DrawDebugSphere(GetWorld(), LeftTop, 10.f, 12, FColor::Red, false, 5.f);
	DrawDebugSphere(GetWorld(), RightBottom, 10.f, 12, FColor::Red, false, 5.f);
	// Calculate width and height based on the bounding box extents
	// 绘制DebugLine， 左上带右上
	DrawDebugLine(GetWorld(), LeftTop, RightTop, FColor::Red, false, 5.f);
	// 绘制DebugLine， 右上带右下
	DrawDebugLine(GetWorld(), RightTop, RightBottom, FColor::Red, false, 5.f);
	auto Width = (LeftTop - RightTop).Size();
	auto Length = (RightTop - RightBottom).Size();
	UE_LOG(LogTemp, Warning, TEXT("Width: %f, Length: %f"), Width, Length);
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
	auto GSettings = GetDefault<UProjectJGeneralSettings>();
	// 落在某一张卡牌上面，需要将该卡牌挤开到一个空位； 卡牌的中心为ActorLocation, 卡牌的Size为GeneralSettings->CardSize
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	// 遍历所有正在使用的卡牌， 看看自己的落点是否会压到其它卡牌
	auto AllCards = ContextSystem->GetUsingCards();
	
	for (auto& Card : AllCards)
	{
		if (Card == this)
		{
			continue;
		}
		// Todo: 这里没有区分是不是执行区， 位置稍有误差
		
		auto CardLocation = Card->GetActorLocation();
		auto CardSize = GSettings->CardSize;
		// Check if the current card overlaps with another card
		if (FMath::Abs(CardLocation.X - GetActorLocation().X) < CardSize.X &&
			FMath::Abs(CardLocation.Y - GetActorLocation().Y) < CardSize.Y)
		{
			// Adjust the position of the overlapping card
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("压到了卡牌"));
			Card->FrameSprite->SetVisibility(true);
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle,
				[Card]()
				{
					Card->FrameSprite->SetVisibility(false);
				},
				3,
				false
			);
		}
	}
	
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

