// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Card/ProjectJCardBase.h"

#include "PaperSpriteComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "Core/ProjectJEditorDebugBFL.h"
#include "Core/DeveloperSettings/ProjectJGeneralSettings.h"
#include "Core/System/ProjectJContextSystem.h"
#include "Game/ProjectJCardLayoutManager.h"
#include "Game/ProjectJLevelSettingActor.h"
#include "Game/Card/ProjectJCardExecuteArea.h"

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
	auto LayoutManager = GetWorld()->GetSubsystem<UProjectJContextSystem>()->CardLayoutManager;
	if (LayoutManager)
    {
        LayoutManager->OnDragCard(ID);
    }
}

void AProjectJCardBase::OnDrop(float InDuration)
{
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	auto Location = GetActorLocation();
	auto SelfDesiredLocation = FVector(Location.X, Location.Y, 0.f);
	IsDropOnExecuteArea = false;
	
	if (!ContextSystem->ExecuteArea->IsExecuting() && ContextSystem->IsInExecuteArea(SelfDesiredLocation))
	{
		// 播放动画， 放置到执行区域， 播放完毕后，调用DropOnExecuteArea(); 执行完毕后才可以执行其它卡牌, 这张卡牌执行完毕后必然会消失， 如果NPC不能交谈，将无法执行；
		// Todo: 如果NPC交谈完毕后，暂定在执行区上方刷新一张新的“交谈过的”NPC卡牌
		GetWorld()->GetTimerManager().SetTimer(
			DropOnGroundTimerHandle,
			this,
			&AProjectJCardBase::UpdateDropOnGroundAnimation,
			0.01f,
			true
		);
		IsDropOnExecuteArea = true;
		DropOnGroundStartTime = GetWorld()->GetTimeSeconds();
		DropOnGroundDuration = InDuration;
		DropOnGroundStartLocation = GetActorLocation();
		
		return;
	}

	bool SimpleDrop = !ContextSystem->LevelSettingActor->UseCardLayoutManager;
	if (ContextSystem->LevelSettingActor->UseCardLayoutManager)
	{
		auto LayoutManager = ContextSystem->CardLayoutManager;
		bool HasLayoutTask = LayoutManager->PlaceCardAndRelayout(
			this,
			InDuration,
			SelfDesiredLocation
		);

		if (!HasLayoutTask)
		{
			SimpleDrop = true;
		}
	}
	
	if (SimpleDrop)
	{
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

	SetActorEnableCollision(true);
}

void AProjectJCardBase::OnCancelDrag()
{
	auto LayoutManager = GetWorld()->GetSubsystem<UProjectJContextSystem>()->CardLayoutManager;
	if (LayoutManager)
	{
		LayoutManager->OnStopDragCard(ID);
	}
	SetActorEnableCollision(true);
	DragStartLocation.Z = 0.f;
	SetActorLocation(DragStartLocation);
	// 旋转重置
	SetActorRotation(FRotator::ZeroRotator);
}

FVector AProjectJCardBase::GetCurrentLocation()
{
	return GetActorLocation();
}

// void AProjectJCardBase::OnSpellFocus()
// {
// 	FrameSprite->SetVisibility(true);
// }
//
// void AProjectJCardBase::OnLoseSpellFocus()
// {
// 	FrameSprite->SetVisibility(false);
// }

void AProjectJCardBase::OnWaitForExecuteSelect()
{
	CanSelect = true;
	FrameSprite->SetVisibility(true);
}

void AProjectJCardBase::EndExecuteSelect()
{
	CanSelect = false;
	FrameSprite->SetVisibility(false);
}

void AProjectJCardBase::HideCard(float Duration)
{
	InitialScale = GetActorScale3D();
	PlayHideAnimation(Duration);
}

void AProjectJCardBase::ShowCard(float Duration)
{
	SetActorScale3D(FVector::ZeroVector);
	InitialScale = FVector::OneVector;
	PlayShowAnimation(Duration);
}

void AProjectJCardBase::PopupCard(const FVector& StartLocation, const FVector& TargetLocation, float Duration)
{
	PlayPopupAnimation(StartLocation, TargetLocation, Duration);
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
		if (IsDropOnExecuteArea)
		{
			auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
			SetActorLocation(ContextSystem->ExecuteArea->GetActorLocation() + FVector(0.f, 0.f, 3.f));
			ContextSystem->ExecuteArea->StartExecute(this);
		}
		else
		{
			SetActorLocation(FVector(DropOnGroundStartLocation.X, DropOnGroundStartLocation.Y, 0.f));
		}
		
		SetActorRotation(FRotator::ZeroRotator);
		// Stop the timer when the animation is complete
		GetWorld()->GetTimerManager().ClearTimer(DropOnGroundTimerHandle);
		Title->SetVisibility(true);
		return;
	}
	
	float Alpha = ElapsedTime / DropOnGroundDuration;
	FVector NewLocation = DropOnGroundStartLocation;
	if (IsDropOnExecuteArea)
	{
		auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
		// Lerp到执行区域的位置
		NewLocation = FMath::Lerp(DropOnGroundStartLocation, ContextSystem->ExecuteArea->GetActorLocation() + FVector(0.f, 0.f, 3.f), Alpha);
	}
	else
	{
		NewLocation.Z = FMath::Lerp(DropOnGroundStartLocation.Z, 0.0f, Alpha);
	}
	
	FRotator CurrentRotation = GetActorRotation();
	FRotator NewRotation = FMath::Lerp(CurrentRotation, FRotator::ZeroRotator, Alpha);

	SetActorLocation(NewLocation);
	SetActorRotation(NewRotation);
}

void AProjectJCardBase::PlayHideAnimation(float Duration)
{
	AnimationStartTime = GetWorld()->GetTimeSeconds();
	AnimationDuration = Duration;
    
	GetWorld()->GetTimerManager().SetTimer(
		CardAnimationTimerHandle,
		this,
		&AProjectJCardBase::UpdateHideAnimation,
		0.016f, // 约60帧
		true
	);
}

void AProjectJCardBase::UpdateHideAnimation()
{
	auto LevelSettings = GetWorld()->GetSubsystem<UProjectJContextSystem>()->LevelSettingActor;
	const auto& Config = LevelSettings->HideAnimConfig;
	float CurrentTime = GetWorld()->GetTimeSeconds();
	float Alpha = (CurrentTime - AnimationStartTime) / Config.Duration;
    
	if (Alpha >= 1.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(CardAnimationTimerHandle);
		SetActorScale3D(FVector::ZeroVector);
		Title->SetVisibility(false);
		return;
	}
    
	float ScaleFactor;
	if (Alpha < Config.ScaleUpPhaseRatio) // 放大阶段
	{
		float SubAlpha = Alpha / Config.ScaleUpPhaseRatio;
		ScaleFactor = FMath::InterpEaseOut(1.0f, Config.MaxScaleFactor, SubAlpha, 2.0f);
	}
	else // 直接缩小消失
	{
		float SubAlpha = (Alpha - Config.ScaleUpPhaseRatio) / (1.0f - Config.ScaleUpPhaseRatio);
		ScaleFactor = FMath::InterpExpoIn(Config.MaxScaleFactor, 0.0f, SubAlpha);
	}
    
	SetActorScale3D(InitialScale * ScaleFactor);
}

void AProjectJCardBase::PlayShowAnimation(float Duration)
{
	AnimationStartTime = GetWorld()->GetTimeSeconds();
	AnimationDuration = Duration;
    
	GetWorld()->GetTimerManager().SetTimer(
		CardAnimationTimerHandle,
		this,
		&AProjectJCardBase::UpdateShowAnimation,
		0.016f,
		true
	);
}

void AProjectJCardBase::UpdateShowAnimation()
{
	auto LevelSettings = GetWorld()->GetSubsystem<UProjectJContextSystem>()->LevelSettingActor;
	const auto& Config = LevelSettings->ShowAnimConfig;
	float CurrentTime = GetWorld()->GetTimeSeconds();
	float Alpha = (CurrentTime - AnimationStartTime) / Config.Duration;
    
	if (Alpha >= 1.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(CardAnimationTimerHandle);
		SetActorScale3D(InitialScale);
		Title->SetVisibility(true);
		return;
	}
    
	float ScaleFactor;
	if (Alpha < Config.ScaleUpPhaseRatio) // 从0放大到超过1
	{
		float SubAlpha = Alpha / Config.ScaleUpPhaseRatio;
		ScaleFactor = FMath::InterpExpoOut(0.0f, Config.MaxScaleFactor, SubAlpha);
	}
	else // 平滑回到1
	{
		float SubAlpha = (Alpha - Config.ScaleUpPhaseRatio) / (1.0f - Config.ScaleUpPhaseRatio);
		ScaleFactor = FMath::InterpEaseOut(Config.MaxScaleFactor, 1.0f, SubAlpha, 2.0f);
	}
    
	SetActorScale3D(InitialScale * ScaleFactor);
}

void AProjectJCardBase::PlayPopupAnimation(const FVector& StartLocation, const FVector& TargetLocation, float Duration)
{
	auto LevelSettings = GetWorld()->GetSubsystem<UProjectJContextSystem>()->LevelSettingActor;
	const auto& Config = LevelSettings->PopSpawnAnimConfig;
	PopupStartLocation = StartLocation;
	PopupTargetLocation = TargetLocation;
    
	// 计算移动方向
	FVector MoveDirection = (TargetLocation - StartLocation).GetSafeNormal();
    
	// 基于移动方向决定旋转的主要方向
	float BaseYaw = FMath::RadiansToDegrees(FMath::Atan2(MoveDirection.Y, MoveDirection.X));
	
	PopupStartRotation = FRotator(
		FMath::RandRange(Config.MinPitch, Config.MaxPitch),          // Pitch
		BaseYaw + FMath::RandRange(Config.MinYaw, Config.MaxYaw),     // Yaw
		FMath::RandRange(Config.MinRoll, Config.MaxRoll)              // Roll
	);

	UE_LOG(LogTemp, Warning, TEXT("PopupStartRotation: %s"), *PopupStartRotation.ToString());
	
	PopupTargetRotation = FRotator::ZeroRotator;
    
	SetActorLocation(StartLocation);
	SetActorRotation(PopupStartRotation);
    
	AnimationStartTime = GetWorld()->GetTimeSeconds();
	AnimationDuration = Duration;
    
	GetWorld()->GetTimerManager().SetTimer(
		CardAnimationTimerHandle,
		this,
		&AProjectJCardBase::UpdatePopupAnimation,
		0.016f,
		true
	);
}

void AProjectJCardBase::UpdatePopupAnimation()
{
	auto LevelSettings = GetWorld()->GetSubsystem<UProjectJContextSystem>()->LevelSettingActor;
	const auto& Config = LevelSettings->PopSpawnAnimConfig;
	float CurrentTime = GetWorld()->GetTimeSeconds();
	float Alpha = (CurrentTime - AnimationStartTime) / AnimationDuration;
    
	if (Alpha >= 1.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(CardAnimationTimerHandle);
		SetActorLocation(PopupTargetLocation);
		SetActorRotation(PopupTargetRotation);
		SetActorScale3D(FVector(1.0f));
		return;
	}

	// 1. 位置动画 - 使用抛物线轨迹
	float HorizontalAlpha = FMath::InterpEaseOut(0.0f, 1.0f, Alpha, 2.0f);
    
	// 计算水平位置
	FVector HorizontalPosition = FMath::Lerp(PopupStartLocation, PopupTargetLocation, HorizontalAlpha);
    
	// 计算垂直位置 - 使用修改后的抛物线
	float HeightOffset = Config.MaxArcHeight * (1.0f - FMath::Square(2.0f * Alpha - 1.0f));
	FVector CurrentLocation = HorizontalPosition + FVector(0, 0, HeightOffset);

	// 2. 旋转动画 - 平滑过渡
	float RotationAlpha = FMath::InterpExpoOut(0.0f, 1.0f, Alpha);
	FRotator CurrentRotation = FMath::Lerp(PopupStartRotation, PopupTargetRotation, RotationAlpha);

	// 3. 缩放动画 - 简单但有效的缩放效果
	float ScaleValue = FMath::InterpExpoOut(0.7f, 1.0f, Alpha);
	FVector CurrentScale = FVector(ScaleValue);

	// 应用变换
	SetActorLocation(CurrentLocation);
	SetActorRotation(CurrentRotation);
	SetActorScale3D(CurrentScale);
}

void AProjectJCardBase::PerformSelected()
{
	
	AnimationStartTime = GetWorld()->GetTimeSeconds();
	AnimationDuration = 0.5f;
	InitialScale = GetActorScale3D();

	GetWorld()->GetTimerManager().SetTimer(
		CardAnimationTimerHandle,
		this,
		&AProjectJCardBase::UpdateSelectedAnimation,
		0.016f,
		true
	);
}

void AProjectJCardBase::UpdateSelectedAnimation()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	float Alpha = (CurrentTime - AnimationStartTime) / AnimationDuration;
	
	if (Alpha >= 1.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(CardAnimationTimerHandle);
		SetActorScale3D(FVector::OneVector);
		return;
	}
	
	float ScaleFactor;
	if (Alpha < 0.5f) // 放大阶段
	{
		float SubAlpha = Alpha / 0.5f;
		ScaleFactor = FMath::InterpEaseOut(1.0f, 1.2f, SubAlpha, 2.0f);
	}
	else // 平滑回到1
	{
		float SubAlpha = (Alpha - 0.5f) / (1.0f - 0.5f);
		ScaleFactor = FMath::InterpEaseOut(1.2f, 1.0f, SubAlpha, 2.0f);
	}
	
	SetActorScale3D(InitialScale * ScaleFactor);
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
                                         const FVector& OtherCardDstLocation, const FVector& CardSize,
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