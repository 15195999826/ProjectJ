// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ProjectJCardLayoutManager.h"

#include "Core/System/ProjectJContextSystem.h"
#include "Game/Card/ProjectJCardBase.h"

#include <chrono>

#include "Core/DeveloperSettings/ProjectJGeneralSettings.h"

// Sets default values
AProjectJCardLayoutManager::AProjectJCardLayoutManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AProjectJCardLayoutManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProjectJCardLayoutManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (FrameRecords.Num() > 0)
	{
		const auto& Record = FrameRecords[0];
		auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
		auto UsingCards = ContextSystem->GetUsingCardsMap();
		for (auto& Pair : Record)
		{
			if (Pair.Key == DraggingCardID)
			{
				continue;
			}
			
			if (Pair.Key == DroppingCardID)
			{
				auto Card = UsingCards.FindRef(Pair.Key);
				if (IsControlCardDrop)
				{
					// 融合运动， 既要下落和调整旋转，也要应用FrameRecord
					
					check(Card);
					float ElapsedTime = GetWorld()->GetTimeSeconds() - DroppingStartTime;
					if (ElapsedTime > DesiredDropDuration)
					{
						IsControlCardDrop = false;
						Card->SetActorLocation(FVector(Pair.Value.X, Pair.Value.Y, 0.f));
						Card->SetActorRotation(FRotator::ZeroRotator);
						continue;
					}
					
					float Alpha = ElapsedTime / DesiredDropDuration;
					auto NewLocation = Pair.Value;
					NewLocation.Z = FMath::Lerp(DropOnGroundStartLocation.Z, 0.0f, Alpha);

					FRotator CurrentRotation = Card->GetActorRotation();
					FRotator NewRotation = FMath::Lerp(CurrentRotation, FRotator::ZeroRotator, Alpha);
					Card->SetActorLocation(NewLocation);
					Card->SetActorRotation(NewRotation);
				}
				else
				{
					Card->SetActorLocation(Pair.Value);
				}
				continue;
			}

			
			if (auto Card = UsingCards.FindRef(Pair.Key))
			{
				Card->SetActorLocation(Pair.Value);
			}
		}
		
		FrameRecords.RemoveAt(0);
	}
	else if (IsControlCardDrop)
	{
		auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
		auto UsingCards = ContextSystem->GetUsingCardsMap();
		if (auto Card = UsingCards.FindRef(DroppingCardID))
		{
			// 控制卡牌下落
			float ElapsedTime = GetWorld()->GetTimeSeconds() - DroppingStartTime;
			FVector NewLocation = Card->GetActorLocation();
			if (ElapsedTime > DesiredDropDuration)
			{
				IsControlCardDrop = false;
				Card->SetActorLocation(FVector(NewLocation.X, NewLocation.Y, 0.f));
				Card->SetActorRotation(FRotator::ZeroRotator);
				return;
			}
			
			float Alpha = ElapsedTime / DesiredDropDuration;
			
			NewLocation.Z = FMath::Lerp(DropOnGroundStartLocation.Z, 0.0f, Alpha);
	
			FRotator CurrentRotation = Card->GetActorRotation();
			FRotator NewRotation = FMath::Lerp(CurrentRotation, FRotator::ZeroRotator, Alpha);

			Card->SetActorLocation(NewLocation);
			Card->SetActorRotation(NewRotation);
		}
	}
}

bool AProjectJCardLayoutManager::PlaceCardAndRelayout(AProjectJCardBase* NewCard, float InDesiredDropDuration, const FVector& NewCardDesiredLocation,
                                                      const FVector2D& DeskBounds, const FVector2D& CardSize)
{
	OnStopDragCard(NewCard->ID);
	DroppingCardID = INT_MIN;
	FrameRecords.Empty();
	// DeskBounds已经处理了卡牌的大小， 是卡牌中心可以在的范围
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
			HasAnyCardOverlapped = true;
			break;
		}
	}

	// 没有卡牌之间有重叠， 不需要调整
	if (!HasAnyCardOverlapped)
	{
		return false;
	}

	Async(EAsyncExecution::TaskGraph, [this, AllCards, NewCard, NewCardDesiredLocation,DeskBounds,CardSize]()
	{
		// 获取开始时间
		auto start = std::chrono::high_resolution_clock::now();
	
		TArray<TMap<int32, FVector>> TaskResult;
		// 计算最优布局
		TMap<AProjectJCardBase*, FVector> Forces;
		TMap<AProjectJCardBase*, FVector> Positions;
		TSet<AProjectJCardBase*> FixedCards;

		// 初始化位置
		for (auto Card : AllCards)
		{
			Positions.Add(Card, Card == NewCard ? NewCardDesiredLocation : Card->GetActorLocation());
			Forces.Add(Card, FVector::ZeroVector);
		}
		bool UnlockNewCardMove = false;
		int32 Iteration = 0;
		while (Iteration < LayoutConfig.MaxIterations)
		{
			// FixedCard 只记录每帧是不是生成了新的
			FixedCards.Empty();
			// 计算各个卡牌受力的方向， 这个方向由与它重叠卡牌的重叠范围决定
			// 重置力
			for (auto& Pair : Forces)
			{
				Pair.Value = FVector::ZeroVector;
			}

			for (auto GetForceCard : AllCards)
			{
				if (GetForceCard == NewCard)
				{
					if (!UnlockNewCardMove)
					{
						continue; // 新卡牌尽可能保持在原位
					}
				}

				const auto& GetForceCardPosition = Positions[GetForceCard];
				for (auto GiveForceCard : AllCards)
				{
					if (GetForceCard == GiveForceCard) continue;

					const auto& GiveForceCardPosition = Positions[GiveForceCard];
					FVector Delta = Positions[GetForceCard] - Positions[GiveForceCard];

					// 只在重叠时产生斥力
					if (IsTwoCardOverlap(GetForceCardPosition, GiveForceCardPosition, CardSize))
					{
						GiveCardForce(GetForceCard, Delta, Forces);
					}
				}

				// 处理边界碰撞
				// 如果超出了左边界， 则向右给与一个力
				HandleBoundaryForces(GetForceCard, GetForceCardPosition, DeskBounds, Forces, FixedCards);
			}

			// 如果存在固定卡牌，则移动新卡牌
			if (FixedCards.Num() > 0)
			{
				// 一旦出现了固定卡牌，新卡牌就可以移动了
				UnlockNewCardMove = true;
				// 检查所有移动的卡牌，是否会被固定卡牌卡住
				// 检查每张可移动的卡牌
				for (auto Tuple : Forces)
				{
					const auto Card = Tuple.Key;
					const FVector& CardPosition = Positions[Card];
					FVector& CardForce = Forces[Card];

					if (!CardForce.IsNearlyZero())
					{
						// 分别检查X和Y方向的力
						FVector ForceX(CardForce.X, 0.0f, 0.0f);
						FVector ForceY(0.0f, CardForce.Y, 0.0f);

						bool BlockedX = false;
						bool BlockedY = false;

						// 检查X方向移动
						if (!ForceX.IsNearlyZero())
						{
							// 首先检查边界阻挡
							FVector PredictedPositionX = CardPosition + ForceX * LayoutConfig.IterationInterval;
							if ((ForceX.X > 0 && PredictedPositionX.X >= DeskBounds.X) ||
								(ForceX.X < 0 && PredictedPositionX.X <= -DeskBounds.X))
							{
								BlockedX = true;
								CardForce.X = 0.0f;
							}

							// 如果没有被边界阻挡，检查是否被固定卡牌阻挡
							if (!BlockedX)
							{
								for (auto FixedCard : FixedCards)
								{
									if (IsTwoCardOverlap(PredictedPositionX, Positions[FixedCard], CardSize))
									{
										FVector DeltaToFixed = Positions[FixedCard] - CardPosition;
										if ((ForceX.X > 0 && DeltaToFixed.X > 0) ||
											(ForceX.X < 0 && DeltaToFixed.X < 0))
										{
											BlockedX = true;
											CardForce.X = 0.0f;
											break;
										}
									}
								}
							}
						}

						// 检查Y方向移动
						if (!ForceY.IsNearlyZero())
						{
							// 首先检查边界阻挡
							FVector PredictedPositionY = CardPosition + ForceY * LayoutConfig.IterationInterval;
							if ((ForceY.Y > 0 && PredictedPositionY.Y >= DeskBounds.Y) ||
								(ForceY.Y < 0 && PredictedPositionY.Y <= -DeskBounds.Y))
							{
								BlockedY = true;
								CardForce.Y = 0.0f;
							}

							// 如果没有被边界阻挡，检查是否被固定卡牌阻挡
							if (!BlockedY)
							{
								for (auto FixedCard : FixedCards)
								{
									if (IsTwoCardOverlap(PredictedPositionY, Positions[FixedCard], CardSize))
									{
										FVector DeltaToFixed = Positions[FixedCard] - CardPosition;
										if ((ForceY.Y > 0 && DeltaToFixed.Y > 0) ||
											(ForceY.Y < 0 && DeltaToFixed.Y < 0))
										{
											BlockedY = true;
											CardForce.Y = 0.0f;
											break;
										}
									}
								}
							}
						}

						// 如果两个方向都被阻挡（无论是被边界还是固定卡牌），则将卡牌标记为固定
						if (BlockedX && BlockedY)
						{
							FixedCards.Add(Card);
							CardForce = FVector::ZeroVector;

							// 添加调试信息
							// UE_LOG(LogTemp, Warning, TEXT("Card %d is completely blocked and marked as fixed"), Card->ID);
						}
						// 如果只有一个方向被阻挡，保留另一个方向的力
						// else if (BlockedX || BlockedY)
						// {
						// 	UE_LOG(LogTemp, Warning, TEXT("Card %d is partially blocked: BlockedX=%d, BlockedY=%d"),
						// 	       Card->ID, BlockedX, BlockedY);
						// }
					}
				}
			}

			// 更新位置
			for (auto Card : AllCards)
			{
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

			TMap<int32,FVector> ID2Positions;
			for (auto Card : AllCards)
			{
				// 如果卡牌被固定， 则不再移动
				ID2Positions.Add(Card->ID, Positions[Card]);
			}
			TaskResult.Add(ID2Positions);

			// 检查是否所有卡牌都不重叠
			bool AllCardsNotOverlap = true;
			for (auto ACard : AllCards)
			{
				const auto& ACardPosition = Positions[ACard];
				for (auto BCard : AllCards)
				{
					if (ACard == BCard) continue;
					if (IsTwoCardOverlap(ACardPosition, Positions[BCard], CardSize))
					{
						AllCardsNotOverlap = false;
						break;
					}
				}
			}

			if (AllCardsNotOverlap)
			{
				UE_LOG(LogTemp, Warning, TEXT("所有卡牌都不重叠，布局完成"));
				break;
			}

			Iteration++;
		}

		// // 获取结束时间
		auto end = std::chrono::high_resolution_clock::now();
		//
		// 计算并打印执行时间, 0.001毫秒级别寻路，稍长一点也只有0.05毫秒
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		UE_LOG(LogTemp, Warning, TEXT("布局执行时间 time: %lld microseconds"), duration.count());

		return TaskResult;
	}).Next([this](const TArray<TMap<int32, FVector>>& TaskResult)
	{
		// 如何在主线程调用ExecDebugFrameStatus？
		AsyncTask(ENamedThreads::GameThread, [this, TaskResult]()
		{
			FrameRecords = TaskResult;
		});
	});
	
	IsControlCardDrop = true;
	DroppingCardID = NewCard->ID;
	DroppingStartTime = GetWorld()->GetTimeSeconds();
	DesiredDropDuration = InDesiredDropDuration;
	DropOnGroundStartLocation = NewCard->GetActorLocation();
	return true;
}

void AProjectJCardLayoutManager::OnDragCard(int32 InCardID)
{
	if (InCardID == DroppingCardID)
	{
		IsControlCardDrop = false;
	}

	DraggingCardID = InCardID;
}

void AProjectJCardLayoutManager::OnStopDragCard(int32 InCardID)
{
	DraggingCardID = INT_MIN;
}

void AProjectJCardLayoutManager::HandleBoundaryForces(AProjectJCardBase* InGetForceCard,
                                                      const FVector& InGetForceCardPosition, const FVector2D& DeskBounds, TMap<AProjectJCardBase*, FVector>& Forces,
                                                      TSet<AProjectJCardBase*>& FixedCards)
{
	// 获取当前的力
	const FVector& CurrentForce = Forces[InGetForceCard];
	
	// 预测下一步位置
	FVector PredictedPosition = InGetForceCardPosition + CurrentForce * LayoutConfig.IterationInterval;
	
	// 检查是否在边界
	bool WillAtLeftBound = PredictedPosition.Y <= -DeskBounds.Y;
	bool WillAtRightBound = PredictedPosition.Y >= DeskBounds.Y;
	bool WillAtTopBound = PredictedPosition.X >= DeskBounds.X;
	bool WillAtBottomBound = PredictedPosition.X <= -DeskBounds.X;
	
	// 如果卡牌同时在多个边界
	if ((WillAtLeftBound || WillAtRightBound) && (WillAtTopBound || WillAtBottomBound))
	{
		// 检查力的方向是否会进一步压向边界
		bool WillPushAgainstBoundary = 
			(WillAtLeftBound && CurrentForce.Y < 0) ||
			(WillAtRightBound && CurrentForce.Y > 0) ||
			(WillAtTopBound && CurrentForce.X > 0) ||
			(WillAtBottomBound && CurrentForce.X < 0);

		if (WillPushAgainstBoundary)
		{
			// 将卡牌标记为固定状态
			FixedCards.Add(InGetForceCard);
			// 清除这张卡的所有力
			Forces[InGetForceCard] = FVector::ZeroVector;
			return;
		}
	}

	// 如果卡牌没有被固定，正常处理边界力
	if (!FixedCards.Contains(InGetForceCard))
	{
		if (WillAtLeftBound && CurrentForce.Y < 0)
		{
			Forces[InGetForceCard].Y  = 0;
		}
		if (WillAtRightBound && CurrentForce.Y > 0)
		{
			Forces[InGetForceCard].Y = 0;
		}
		if (WillAtTopBound && CurrentForce.X > 0)
		{
			Forces[InGetForceCard].X = 0;
		}
		if (WillAtBottomBound && CurrentForce.X < 0)
		{
			Forces[InGetForceCard].X = 0;
		}
	}
}

void AProjectJCardLayoutManager::GiveCardForce(AProjectJCardBase* InCard, FVector& InDelta, TMap<AProjectJCardBase*, FVector>& Forces)
{
	if (InDelta.IsNearlyZero())
	{
		InDelta = FVector(FMath::RandRange(-1.0f, 1.0f), FMath::RandRange(-1.0f, 1.0f), 0.0f);
	}

	// 计算X和Y方向的排斥力
	FVector Force = InDelta.GetSafeNormal();
	Force.X *= LayoutConfig.BaseRepulsionForce;
	Force.Y *= LayoutConfig.BaseRepulsionForce;
	Force.Z = 0.0f;

	Forces[InCard] += Force;
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
