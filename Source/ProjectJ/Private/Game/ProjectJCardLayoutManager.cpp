// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ProjectJCardLayoutManager.h"

#include "Core/System/ProjectJContextSystem.h"
#include "Game/Card/ProjectJCardBase.h"

#include <chrono>

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

bool AProjectJCardLayoutManager::PlaceCardAndRelayout(AProjectJCardBase* NewCard, const FVector& NewCardDesiredLocation,
	const FVector2D& DeskBounds, const FVector2D& CardSize)
{
	// DeskBounds已经处理了卡牌的大小， 是卡牌中心可以在的范围
	// 获取开始时间
	auto start = std::chrono::high_resolution_clock::now();
	
	// 获取所有卡牌
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	TArray<TObjectPtr<AProjectJCardBase>> AllCards = ContextSystem->GetUsingCards();
	bool HasAnyCardOverlapped = false;
	// 检查NewCard是否跟其它卡牌之间有重叠
	for (auto Card : AllCards)
	{
		if (Card == NewCard)
		{
			continue;
		}

		auto CardLocation = Card->GetActorLocation();
		// Check if the current card overlaps with another card
		if (IsTwoCardOverlap(CardLocation, NewCardDesiredLocation, CardSize))
		{
			// 在重叠卡牌位置DrawDebugSphere
			DrawDebugSphere(GetWorld(), CardLocation, 10.f, 12, FColor::Red, false, 5.f);
			HasAnyCardOverlapped = true;
			break;
		}
	}

	// 没有卡牌之间有重叠， 不需要调整
	if (!HasAnyCardOverlapped)
	{
		return false;
	}
	
	// 计算最优布局
	TMap<AProjectJCardBase*, FVector> Forces;
	TMap<AProjectJCardBase*, FVector> Positions;

	// 初始化位置
	for (auto Card : AllCards)
	{
		Positions.Add(Card, Card == NewCard ? NewCardDesiredLocation : Card->GetActorLocation());
		Forces.Add(Card, FVector::ZeroVector);
	}

	DebugFrameStatus.Empty();
	int32 Iteration = 0;
	while (Iteration < LayoutConfig.MaxIterations)
	{
		// 计算各个卡牌受力的方向， 这个方向由与它重叠卡牌的重叠范围决定
		// 重置力
		for (auto& Pair : Forces)
		{
			Pair.Value = FVector::ZeroVector;
		}

		for (auto GetForceCard : AllCards)
		{
			if (GetForceCard == NewCard) continue; // 新卡牌保持在原位

			const auto& GetForceCardPosition = Positions[GetForceCard];
			for (auto GiveForceCard : AllCards)
			{
				if (GetForceCard == GiveForceCard) continue;

				const auto& GiveForceCardPosition = Positions[GiveForceCard];
				FVector Delta = Positions[GetForceCard] - Positions[GiveForceCard];
                
				// 只在重叠时产生斥力
				if (IsTwoCardOverlap(GetForceCardPosition, GiveForceCardPosition, CardSize))
                {
                    // 防止除零
                    if (Delta.IsNearlyZero())
                    {
                        Delta = FVector(FMath::RandRange(-1.0f, 1.0f), FMath::RandRange(-1.0f, 1.0f), 0.0f);
                    }

                    // 计算X和Y方向的排斥力
                    FVector Force = Delta.GetSafeNormal();
                    Force.X *= LayoutConfig.BaseRepulsionForce;
                    Force.Y *= LayoutConfig.BaseRepulsionForce;
                    Force.Z = 0.0f;

                    Forces[GetForceCard] += Force;
                }
			}

			// 处理边界碰撞
			// 如果超出了左边界， 则向右给与一个力
			if (GetForceCardPosition.Y< -DeskBounds.Y)
			{
				Forces[GetForceCard].Y += LayoutConfig.BoundaryForce;
			}
			// 右边界
			if (GetForceCardPosition.Y > DeskBounds.Y)
			{
				Forces[GetForceCard].Y -= LayoutConfig.BoundaryForce;
			}
			// 上边界
			if (GetForceCardPosition.X > DeskBounds.X)
			{
				Forces[GetForceCard].X -= LayoutConfig.BoundaryForce;
			}
			// 下边界
			if (GetForceCardPosition.X < -DeskBounds.X)
			{
				Forces[GetForceCard].X += LayoutConfig.BoundaryForce;
			}
		}

		// 检查是否所有卡牌受力都为0
		bool AllCardNoForce = true;
		for (const auto& Tuple : Forces)
		{
			if (!Tuple.Value.IsNearlyZero())
			{
				AllCardNoForce = false;
				break;
			}
		}

		if (AllCardNoForce)
		{
			// 打印当前遍历的次数，与因为所有卡牌不受力中断
			UE_LOG(LogTemp, Warning, TEXT("Iteration: %d, All cards have no force!"), Iteration);
			break;
		}

		// 更新位置

		for (auto Card : AllCards)
		{
			if (Card == NewCard)
			{
				check(Forces[Card].IsNearlyZero());
				continue; // 新卡牌保持在原位
			}

			// 只有在有力的情况下才移动
			if (!Forces[Card].IsNearlyZero())
			{
				FVector NewPosition = Positions[Card] + Forces[Card] * LayoutConfig.IterationInterval;
				// 确保在桌面范围内
				NewPosition.X = FMath::Clamp(NewPosition.X, 
					-DeskBounds.X,
					DeskBounds.X);
				NewPosition.Y = FMath::Clamp(NewPosition.Y, 
					-DeskBounds.Y,
					DeskBounds.Y);
				NewPosition.Z = 0.0f;
				Positions[Card] = NewPosition;
			}
		}
		DebugFrameStatus.Add(Positions);
		Iteration++;
	}

	// // 获取结束时间
	auto end = std::chrono::high_resolution_clock::now();
	//
	// 计算并打印执行时间, 0.001毫秒级别寻路，稍长一点也只有0.05毫秒
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	UE_LOG(LogTemp, Warning, TEXT("布局执行时间 time: %lld microseconds"), duration.count());

	// DebugFrameStatus 每帧应用一次，观察卡牌的移动情况， 用Timer实现
	DebugCursor = 0;
	ExecDebugFrameStatus();
	

	// 应用新布局
	// for (auto& Timer : MoveTimers)
	// {
	// 	GetWorld()->GetTimerManager().ClearTimer(Timer);
	// }
	// MoveTimers.Empty();
	//
	// float StartTime = GetWorld()->GetTimeSeconds();
	// for (const auto& Pair : NewPositions)
	// {
	// 	if (Pair.Key != NewCard)
	// 	{
	// 		FTimerHandle TimerHandle;
	// 		// 1s内Lerp到新位置
	// 		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Pair, StartTime]()
	// 		{
	// 			float ElapsedTime = FMath::Clamp(GetWorld()->GetTimeSeconds() - StartTime, 0.0f, 1.0f);
	// 			Pair.Key->SetActorLocation(FMath::Lerp(Pair.Key->GetActorLocation(), Pair.Value, ElapsedTime));
	// 		}, 0.01f, true);
	// 		MoveTimers.Add(TimerHandle);
	// 	}
	// }
	// FTimerHandle EndMoveTimerHandle;
	// GetWorld()->GetTimerManager().SetTimer(EndMoveTimerHandle, [this, NewPositions, NewCard]()
	// {
	// 	for (auto& Timer : MoveTimers)
	// 	{
	// 		GetWorld()->GetTimerManager().ClearTimer(Timer);
	// 	}
	//
	// 	for (const auto& Pair : NewPositions)
	// 	{
	// 		if (Pair.Key != NewCard)
	// 		{
	// 			Pair.Key->SetActorLocation(Pair.Value);
	// 		}
	// 	}
	// }, 1.0f, false);

	return true;
}

void AProjectJCardLayoutManager::ExecDebugFrameStatus()
{
	if (DebugCursor >= DebugFrameStatus.Num())
	{
		return;
	}
	
	const auto& FrameStatus = DebugFrameStatus[DebugCursor];
	for (const auto& Pair : FrameStatus)
	{
		Pair.Key->SetActorLocation(Pair.Value);
	}
	DebugCursor++;
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
	{
		ExecDebugFrameStatus();
	});
}

bool AProjectJCardLayoutManager::IsPositionInBounds(const FVector& Position, const FVector2D& DeskBounds)
{
	return Position.X >= -DeskBounds.X && Position.X <= DeskBounds.X &&
		   Position.Y >= -DeskBounds.Y && Position.Y <= DeskBounds.Y;
}

bool AProjectJCardLayoutManager::IsTwoCardOverlap(const FVector& ACardLocation, const FVector& BCardLocation,
	const FVector2D& CardSize)
{
	return FMath::Abs(ACardLocation.X - BCardLocation.X) < CardSize.X &&
			FMath::Abs(ACardLocation.Y - BCardLocation.Y) < CardSize.Y;
}
