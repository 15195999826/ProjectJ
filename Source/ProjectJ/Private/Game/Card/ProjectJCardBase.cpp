// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Card/ProjectJCardBase.h"

#include "PaperSpriteComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "Core/DeveloperSettings/ProjectJGeneralSettings.h"
#include "Core/System/ProjectJContextSystem.h"
#include "Game/ProjectJCardLayoutManager.h"
#include "Game/ProjectJLevelSettingActor.h"

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
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	auto Location = GetActorLocation();
	auto SelfDesiredLocation = FVector(Location.X, Location.Y, 0.f);
	auto GSettings = GetDefault<UProjectJGeneralSettings>();
	const auto& CardSize3D = GSettings->CardSize;
	const FVector2D CardSize = FVector2D(CardSize3D.X, CardSize3D.Y);
	// -Y更左， +Y更右， -X更下， +X更上
	const auto& DeskTopSize = GSettings->DeskTopSize;

	const float MaxX = (DeskTopSize.X - CardSize3D.X) * 0.5f;
	const float MaxY = (DeskTopSize.Y - CardSize3D.Y) * 0.5f;
	const FVector2D DeskBounds = FVector2D(MaxX, MaxY);
	// 以（0，0，0）为中心，Debug绘制一个DeskTopSize的矩形
	DrawDebugBox(GetWorld(), FVector(0.f, 0.f, 0.f), DeskTopSize * 0.5f, FColor::Green, false, 5.f);

	if (ContextSystem->LevelSettingActor->UseCardLayoutManager)
	{
		auto LayoutManager = ContextSystem->CardLayoutManager;
		bool Success = LayoutManager->PlaceCardAndRelayout(
			this,
			SelfDesiredLocation,
			DeskBounds,
			CardSize
		);
	}
	else
	{
		// 落在某一张卡牌上面，需要将该卡牌挤开到一个空位； 卡牌的中心为ActorLocation, 卡牌的Size为GeneralSettings->CardSize

		// 遍历所有正在使用的卡牌， 看看自己的落点是否会压到其它卡牌
		auto AllCards = ContextSystem->GetUsingCards();

		TArray<TObjectPtr<AProjectJCardBase>> OverlappedCards;
		for (auto& Card : AllCards)
		{
			if (Card == this)
			{
				continue;
			}
			// Todo: 这里没有区分是不是执行区， 位置稍有误差

			auto CardLocation = Card->GetActorLocation();
			// Check if the current card overlaps with another card
			if (AProjectJCardLayoutManager::IsTwoCardOverlap(CardLocation, SelfDesiredLocation, CardSize))
			{
				OverlappedCards.Add(Card);
			}
		}

		if (OverlappedCards.Num() > 0)
		{
			
			for (auto& Card : OverlappedCards)
			{
				// Calculate the direction to move the card
				FVector MoveDirection = Card->GetActorLocation() - SelfDesiredLocation;
				MoveDirection.Z = 0.f;
				MoveDirection.Normalize();
				const float RequiredDistance = CalculateRequiredDistance(CardSize, MoveDirection);
				FVector DstLocation = SelfDesiredLocation + MoveDirection * (RequiredDistance + 0.1f);
				// 检查是否超出边界
				if (AProjectJCardLayoutManager::IsPositionInBounds(DstLocation, DeskBounds))
				{
					Card->SetActorLocation(DstLocation);
					continue;
				}

				// 优先处理特殊边界情况
				FVector BorderCaseDstPosition;
				if (HandleBorderCase(Card->GetActorLocation(), SelfDesiredLocation, DstLocation, CardSize, DeskBounds,
				                     BorderCaseDstPosition))
				{
					Card->SetActorLocation(BorderCaseDstPosition);
					continue;
				}

				// 确定旋转方向并尝试找到有效位置
				bool RotateClockwise = DetermineRotateClockwise(Card->GetActorLocation(), SelfDesiredLocation,
				                                                DstLocation,
				                                                DeskBounds);
				if (FindValidPositionByRotation(SelfDesiredLocation, MoveDirection, CardSize, DeskBounds,
				                                RotateClockwise,
				                                DstLocation))
				{
					Card->SetActorLocation(DstLocation);
					continue;
				}
				// 没有找到可以放置的位置， Todo:
			}
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

float AProjectJCardBase::CalculateRequiredDistance(const FVector2D& CardSize, const FVector& Direction) const
{
	float DirX = FMath::Abs(Direction.X);
	float DirY = FMath::Abs(Direction.Y);
    
	if (DirX > 0.0f && DirY > 0.0f)
	{
		return FMath::Min(CardSize.X / DirX, CardSize.Y / DirY);
	}
	return DirX > 0.0f ? CardSize.X / DirX : CardSize.Y / DirY;
}

bool AProjectJCardBase::HandleBorderCase(const FVector& OtherCardPosition, const FVector& SelfDesiredLocation,
                                         const FVector& OtherCardDstLocation, const FVector2D& CardSize,
                                         const FVector2D& DeskBounds,
                                         FVector& OutDstPosition) const
{
	constexpr float BorderThreshold = 0.2f;
	float ThresholdX = CardSize.X * BorderThreshold;
	float ThresholdY = CardSize.Y * BorderThreshold;

	// 获取A卡当前位置
	bool SpecialBorderCase = false;
	OutDstPosition = OtherCardDstLocation;

	// 计算新卡牌和当前卡牌之间需要的最小分离距离
	float MinSeparationX = CardSize.X;
	float MinSeparationY = CardSize.Y;

	auto MaxX = DeskBounds.X;
	auto MinX = -DeskBounds.X;
	auto MaxY = DeskBounds.Y;
	auto MinY = -DeskBounds.Y;

	// 检查是否接近边界的特殊情况
	if (FMath::Abs(OtherCardPosition.X - MaxX) <= ThresholdX && OtherCardDstLocation.X > MaxX)
	{
		// 接近上边界的情况
		OutDstPosition.X = MaxX;
		// 计算Y轴坐标以避免重叠
		if (SelfDesiredLocation.Y > OtherCardPosition.Y)
		{
			// 新卡在右边，A卡需要往左移
			OutDstPosition.Y = SelfDesiredLocation.Y - MinSeparationY;
		}
		else
		{
			// 新卡在左边，A卡需要往右移
			OutDstPosition.Y = SelfDesiredLocation.Y + MinSeparationY;
		}
		SpecialBorderCase = true;
		// UE_LOG(LogTemp, Warning, TEXT("Card is near top border, snapping to top"));
	}
	else if (FMath::Abs(OtherCardPosition.X - MinX) <= ThresholdX && OtherCardDstLocation.X < MinX)
	{
		// 接近下边界的情况
		OutDstPosition.X = MinX;
		// 计算Y轴坐标以避免重叠
		if (SelfDesiredLocation.Y > OtherCardPosition.Y)
		{
			// 新卡在右边，A卡需要往左移
			OutDstPosition.Y = SelfDesiredLocation.Y - MinSeparationY;
		}
		else
		{
			// 新卡在左边，A卡需要往右移
			OutDstPosition.Y = SelfDesiredLocation.Y + MinSeparationY;
		}
		SpecialBorderCase = true;
		// UE_LOG(LogTemp, Warning, TEXT("Card is near bottom border, snapping to bottom"));
	}
	else if (FMath::Abs(OtherCardPosition.Y - MaxY) <= ThresholdY && OtherCardDstLocation.Y > MaxY)
	{
		// 接近右侧边界的情况
		OutDstPosition.Y = MaxY;
		// 计算X轴坐标以避免重叠
		if (SelfDesiredLocation.X > OtherCardPosition.X)
		{
			// 新卡在上方，A卡需要往下移
			OutDstPosition.X = SelfDesiredLocation.X - MinSeparationX;
		}
		else
		{
			// 新卡在下方，A卡需要往上移
			OutDstPosition.X = SelfDesiredLocation.X + MinSeparationX;
		}
		SpecialBorderCase = true;
		// UE_LOG(LogTemp, Warning, TEXT("Card is near right border, snapping to right"));
	}
	else if (FMath::Abs(OtherCardPosition.Y - MinY) <= ThresholdY && OtherCardDstLocation.Y < MinY)
	{
		// 接近左侧边界的情况
		OutDstPosition.Y = MinY;
		// 计算X轴坐标以避免重叠
		if (SelfDesiredLocation.X > OtherCardPosition.X)
		{
			// 新卡在上方，A卡需要往下移
			OutDstPosition.X = SelfDesiredLocation.X - MinSeparationX;
		}
		else
		{
			// 新卡在下方，A卡需要往上移
			OutDstPosition.X = SelfDesiredLocation.X + MinSeparationX;
		}
		SpecialBorderCase = true;
		// UE_LOG(LogTemp, Warning, TEXT("Card is near left border, snapping to left"));
	}

	if (SpecialBorderCase)
	{
		// 确保边界位置在合法范围内
		OutDstPosition.X = FMath::Clamp(OutDstPosition.X, MinX, MaxX);
		OutDstPosition.Y = FMath::Clamp(OutDstPosition.Y, MinY, MaxY);

		// 检查两张卡是否发生了重叠
		if (AProjectJCardLayoutManager::IsTwoCardOverlap(OutDstPosition,SelfDesiredLocation, CardSize))
		{
			// 未找到合适的位置
			return false;
		}

		// 检查新位置是否在牌桌内
		if (AProjectJCardLayoutManager::IsPositionInBounds(OutDstPosition, DeskBounds))
		{
			return true;
		}
	}

	return false;
}

bool AProjectJCardBase::DetermineRotateClockwise(const FVector& OtherCardPosition, const FVector& SelfDesiredLocation,
	const FVector& OtherCardDstLocation, const FVector2D& DeskBounds)
{
	// 确定旋转方向
	bool RotateClockwise = false;

	auto MaxX = DeskBounds.X;
	auto MinX = -DeskBounds.X;
	auto MaxY = DeskBounds.Y;
	auto MinY = -DeskBounds.Y;

	// 获取Card当前的位置相对于SelfDesiredLocation的方向
	FVector RelativePosition = OtherCardPosition - SelfDesiredLocation;
	// 在Card->GetActorLocation()和SelfDesiredLocation之间绘制DebugLine和DebugSphere
	// DrawDebugLine(GetWorld(), SelfDesiredLocation, OtherCardPosition, FColor::Blue, false, 5.0f);
	// DrawDebugSphere(GetWorld(), OtherCardPosition, 10.0f, 12, FColor::Red, false, 5.0f);
	// DrawDebugSphere(GetWorld(), SelfDesiredLocation, 10.0f, 12, FColor::Green, false, 5.0f);
	// 打印RelativePosition
	UE_LOG(LogTemp, Warning, TEXT("SelfDesiredLocation: %s, CardLocation: %s"),
		   *SelfDesiredLocation.ToString(), *OtherCardPosition.ToString());
	UE_LOG(LogTemp, Warning,
		   TEXT("RelativePosition: %s, NewLocation: %s, MaxX: %f, MaxY: %f, MinX: %f, MinY: %f"),
		   *RelativePosition.ToString(), *OtherCardDstLocation.ToString(), MaxX, MaxY, MinX, MinY);

	// 检查卡牌超出边界的方向和相对位置关系
	if (OtherCardDstLocation.X > MaxX) // 超出下边界
	{
		// 如果卡牌在目标位置左边（Y > 0），则顺时针旋转
		RotateClockwise = (RelativePosition.Y > 0);
	}
	else if (OtherCardDstLocation.X < MinX) // 超出上边界
	{
		// 如果卡牌在目标位置右边（Y < 0），则顺时针旋转
		RotateClockwise = (RelativePosition.Y < 0);
	}
	else if (OtherCardDstLocation.Y > MaxY) // 超出右边界
	{
		// 如果卡牌在目标位置上方（X < 0），则顺时针旋转
		RotateClockwise = (RelativePosition.X < 0);
	}
	else if (OtherCardDstLocation.Y < MinY) // 超出左边界
	{
		// 如果卡牌在目标位置下方（X > 0），则顺时针旋转
		RotateClockwise = (RelativePosition.X > 0);
	}
	
	return RotateClockwise;
}

bool AProjectJCardBase::FindValidPositionByRotation(const FVector& SelfDesiredLocation, const FVector& MoveDirection,
                                                    const FVector2D& CardSize, const FVector2D& DeskBounds, bool RotateClockwise, FVector& OutNewLocation) const
{
	// 尝试旋转寻找合适的位置
	const float RotationStep = FMath::DegreesToRadians(10.0f); // 10度转换为弧度
	const float MaxRotation = 2.0f * PI; // 最大旋转360度
	float CurrentRotation = 0.0f;
	FVector RotatedDirection = MoveDirection;
	// 用于记录最佳位置
	bool FoundValidPosition = false;

	while (CurrentRotation < MaxRotation && !FoundValidPosition)
	{
		float Angle = RotateClockwise ? RotationStep : -RotationStep;
		float CosAngle = FMath::Cos(Angle);
		float SinAngle = FMath::Sin(Angle);

		// 2D向量旋转
		FVector NewDirection(
			RotatedDirection.X * CosAngle - RotatedDirection.Y * SinAngle,
			RotatedDirection.X * SinAngle + RotatedDirection.Y * CosAngle,
			0.0f
		);
		NewDirection.Normalize();
		// 重新计算RequiredDistance
		auto RequiredDistance = CalculateRequiredDistance(CardSize, NewDirection);

		// 计算新位置
		OutNewLocation = SelfDesiredLocation + NewDirection * (RequiredDistance + 0.1f);

		// 检查是否在边界内
		if (AProjectJCardLayoutManager::IsPositionInBounds(OutNewLocation, DeskBounds))
		{
			FoundValidPosition = true;
			// DrawDebugLine(GetWorld(), SelfDesiredLocation, SelfDesiredLocation + NewDirection * 100.0f,
			//   FColor::Green, false, 5.0f);
			break;
		}
		// 沿着测试的移动方向，DrawDebugLine
		// DrawDebugLine(GetWorld(), SelfDesiredLocation, SelfDesiredLocation + NewDirection * 100.0f,
		//               FColor::Red, false, 5.0f);

		RotatedDirection = NewDirection;
		CurrentRotation += RotationStep;
	}

	if (!FoundValidPosition)
	{
		UE_LOG(LogTemp, Error, TEXT("旋转查找无法找到合适的位置放置卡牌！"));
		return false;
	}

	return true;
}