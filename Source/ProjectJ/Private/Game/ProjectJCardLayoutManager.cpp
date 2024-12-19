// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ProjectJCardLayoutManager.h"

#include "Core/System/ProjectJContextSystem.h"
#include "Game/Card/ProjectJCardBase.h"

// Sets default values
AProjectJCardLayoutManager::AProjectJCardLayoutManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AProjectJCardLayoutManager::BeginPlay()
{
	Super::BeginPlay();
	
}

bool AProjectJCardLayoutManager::PlaceCardAndRelayout(class AProjectJCardBase* NewCard, const FVector& DesiredLocation,
	const FVector2D& DeskBounds, const FVector2D& CardSize)
{
	// 获取所有卡牌
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	TArray<TObjectPtr<AProjectJCardBase>> AllCards = ContextSystem->GetUsingCards();
        
	// 计算最优布局
	TMap<AProjectJCardBase*, FVector> NewPositions = 
		CalculateOptimalLayout(AllCards, NewCard, DesiredLocation, DeskBounds, CardSize, LayoutConfig);

	// 应用新布局
	MoveTimers.Empty();
	float StartTime = GetWorld()->GetTimeSeconds();
	for (const auto& Pair : NewPositions)
	{
		if (Pair.Key != NewCard)
		{
			FTimerHandle TimerHandle;
			// 1s内Lerp到新位置
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Pair, StartTime]()
			{
				float ElapsedTime = FMath::Clamp(GetWorld()->GetTimeSeconds() - StartTime, 0.0f, 1.0f);
				Pair.Key->SetActorLocation(FMath::Lerp(Pair.Key->GetActorLocation(), Pair.Value, ElapsedTime));
			}, 0.01f, true);
			MoveTimers.Add(TimerHandle);
		}
	}
	FTimerHandle EndMoveTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(EndMoveTimerHandle, [this, NewPositions, NewCard]()
	{
		for (auto& Timer : MoveTimers)
		{
			GetWorld()->GetTimerManager().ClearTimer(Timer);
		}

		for (const auto& Pair : NewPositions)
		{
			if (Pair.Key != NewCard)
			{
				Pair.Key->SetActorLocation(Pair.Value);
			}
		}
	}, 1.0f, false);

	return true;
}

TMap<AProjectJCardBase*, FVector> AProjectJCardLayoutManager::CalculateOptimalLayout(
	const TArray<AProjectJCardBase*>& AllCards, AProjectJCardBase* NewCard, const FVector& DesiredLocation,
	const FVector2D& DeskBounds, const FVector2D& CardSize, const FLayoutConfig& Config)
{
	TMap<AProjectJCardBase*, FVector> Forces;
	TMap<AProjectJCardBase*, FVector> Positions;
	TMap<AProjectJCardBase*, FVector> Velocities;

	// 初始化位置和速度
	for (auto Card : AllCards)
	{
		Positions.Add(Card, Card == NewCard ? DesiredLocation : Card->GetActorLocation());
		Velocities.Add(Card, FVector::ZeroVector);
	}

	float MaxMovement = Config.ConvergenceThreshold + 1.0f;
	int32 Iteration = 0;

	// 迭代优化直到收敛或达到最大迭代次数
	while (MaxMovement > Config.ConvergenceThreshold && Iteration < Config.MaxIterations)
	{
		Forces.Empty();
		for (auto Card : AllCards)
		{
			Forces.Add(Card, FVector::ZeroVector);
		}

		// 计算卡牌间的排斥力
		for (auto Card1 : AllCards)
		{
			for (auto Card2 : AllCards)
			{
				if (Card1 == Card2) continue;

				FVector Delta = Positions[Card1] - Positions[Card2];
				if (Delta.IsNearlyZero()) continue;

				// 分别计算X和Y方向的力
				float DistanceX = FMath::Abs(Delta.X);
				float DistanceY = FMath::Abs(Delta.Y);

				// 根据卡牌尺寸调整排斥力
				float ForceX = Config.BaseRepulsionForce * CardSize.X / FMath::Max(DistanceX * DistanceX, 1.0f);
				float ForceY = Config.BaseRepulsionForce * CardSize.Y / FMath::Max(DistanceY * DistanceY, 1.0f);

				// 应用方向系数
				ForceX *= Config.DirectionalForceRatio;

				FVector Force(
					ForceX * FMath::Sign(Delta.X),
					ForceY * FMath::Sign(Delta.Y),
					0.0f
				);

				Forces[Card1] += Force;
				Forces[Card2] -= Force;
			}

			// 计算边界排斥力
			FVector CurrentPos = Positions[Card1];
			FVector2D HalfCard = CardSize * 0.5f;
			float BoundaryEffect = Config.BoundaryEffectRatio * FMath::Min(CardSize.X, CardSize.Y);

			// 左边界
			if (CurrentPos.Y - HalfCard.Y < -DeskBounds.Y)
			{
				float Distance = FMath::Abs(CurrentPos.Y + DeskBounds.Y);
				Forces[Card1].Y += Config.BoundaryForce / FMath::Max(Distance * Distance, BoundaryEffect);
			}
			// 右边界
			if (CurrentPos.Y + HalfCard.Y > DeskBounds.Y)
			{
				float Distance = FMath::Abs(CurrentPos.Y - DeskBounds.Y);
				Forces[Card1].Y -= Config.BoundaryForce / FMath::Max(Distance * Distance, BoundaryEffect);
			}
			// 上边界
			if (CurrentPos.X + HalfCard.X > DeskBounds.X)
			{
				float Distance = FMath::Abs(CurrentPos.X - DeskBounds.X);
				Forces[Card1].X -= Config.BoundaryForce / FMath::Max(Distance * Distance, BoundaryEffect);
			}
			// 下边界
			if (CurrentPos.X - HalfCard.X < -DeskBounds.X)
			{
				float Distance = FMath::Abs(CurrentPos.X + DeskBounds.X);
				Forces[Card1].X += Config.BoundaryForce / FMath::Max(Distance * Distance, BoundaryEffect);
			}

			// 计算向目标位置的吸引力
			FVector TargetPos = Card1 == NewCard ? DesiredLocation : Card1->GetActorLocation();
			FVector ToTarget = TargetPos - Positions[Card1];
			Forces[Card1] += ToTarget * Config.AttractionForce;
		}

		// 更新位置
		MaxMovement = 0.0f;
		const float DeltaTime = 0.1f; // 模拟时间步长
		const float Damping = 0.8f; // 阻尼系数

		for (auto Card : AllCards)
		{
			// 更新速度(加入阻尼)
			Velocities[Card] = (Velocities[Card] + Forces[Card] * DeltaTime) * Damping;

			// 更新位置
			FVector NewPosition = Positions[Card] + Velocities[Card] * DeltaTime;

			// 确保在桌面范围内
			NewPosition.X = FMath::Clamp(NewPosition.X, -DeskBounds.X + CardSize.X * 0.5f,
			                             DeskBounds.X - CardSize.X * 0.5f);
			NewPosition.Y = FMath::Clamp(NewPosition.Y, -DeskBounds.Y + CardSize.Y * 0.5f,
			                             DeskBounds.Y - CardSize.Y * 0.5f);
			NewPosition.Z = 0.0f;

			float Movement = (NewPosition - Positions[Card]).Size();
			MaxMovement = FMath::Max(MaxMovement, Movement);

			Positions[Card] = NewPosition;
		}

		Iteration++;
	}

	return Positions;
}
