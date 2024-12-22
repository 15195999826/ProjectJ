// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ProjectJCardLayoutManager.h"

#include "Core/System/ProjectJContextSystem.h"
#include "Game/Card/ProjectJCardBase.h"

#include <chrono>
#include "Core/DeveloperSettings/ProjectJGeneralSettings.h"
#include "Game/Card/ProjectJCardExecuteArea.h"

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

bool AProjectJCardLayoutManager::PlaceCardAndRelayout(AProjectJCardBase* NewCard, float InDesiredDropDuration, const FVector& NewCardDesiredLocation)
{
	auto GSettings = GetDefault<UProjectJGeneralSettings>();
	const auto& CardSize = GSettings->CardSize;
	// -Y更左， +Y更右， -X更下， +X更上
	const auto& DeskTopSize = GSettings->DeskTopSize;

	const float MaxX = (DeskTopSize.X - CardSize.X) * 0.5f;
	const float MaxY = (DeskTopSize.Y - CardSize.Y) * 0.5f;
	const FVector2D DeskBounds = FVector2D(MaxX, MaxY);
	// 以（0，0，0）为中心，Debug绘制一个DeskTopSize的矩形
	DrawDebugBox(GetWorld(), FVector(0.f, 0.f, 0.f), DeskTopSize * 0.5f, FColor::Green, false, 5.f);
	
	OnStopDragCard(NewCard->ID);
	DroppingCardID = INT_MIN;
	// DeskBounds已经处理了卡牌的大小， 是卡牌中心可以在的范围
	// 获取所有卡牌
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	TArray<TObjectPtr<AProjectJCardBase>> AllCards = ContextSystem->GetUsingCards();
	if (ContextSystem->ExecuteArea->ExecutingCard.IsValid())
	{
		AllCards.Remove(ContextSystem->ExecuteArea->ExecutingCard.Get());
	}
	
	bool HasAnyCardOverlapped = ContextSystem->IsOverlapExecuteArea(NewCardDesiredLocation);

	if (!HasAnyCardOverlapped)
	{
		// 检查NewCard是否跟其它卡牌之间有重叠
		for (auto Card : AllCards)
		{
			auto CardLocation = Card->GetActorLocation();
			if (!IsPositionInBounds(CardLocation, DeskBounds))
			{
				HasAnyCardOverlapped = true;
				continue;
			}
		
			if (Card == NewCard)
			{
				continue;
			}

			// Check if the current card overlaps with another card
			if (IsTwoCardOverlap(CardLocation, NewCardDesiredLocation, CardSize))
			{
				HasAnyCardOverlapped = true;
				break;
			}
		}
	}
	
	// 没有卡牌之间有重叠， 不需要调整
	if (!HasAnyCardOverlapped)
	{
		return false;
	}

	
	Async(EAsyncExecution::TaskGraph, [this, AllCards, NewCard, NewCardDesiredLocation,DeskBounds,CardSize]()
	{
		auto LocalContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
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

			if (!IsPositionInBounds(Positions[NewCard], DeskBounds))
			{
				const auto& CardPosition = Positions[NewCard];
				// 放置的卡牌超出了边界， 边界需要给予斥力
				FVector ExceedDistance(0.0f, 0.0f, 0.0f);

				// Check X boundaries
				if (CardPosition.X > DeskBounds.X)
				{
					ExceedDistance.X = CardPosition.X - DeskBounds.X;
				}
				else if (CardPosition.X < -DeskBounds.X)
				{
					ExceedDistance.X = CardPosition.X + DeskBounds.X;
				}
				// Check Y boundaries
				if (CardPosition.Y > DeskBounds.Y)
				{
					ExceedDistance.Y = CardPosition.Y - DeskBounds.Y;
				}
				else if (CardPosition.Y < -DeskBounds.Y)
				{
					ExceedDistance.Y = CardPosition.Y + DeskBounds.Y;
				}

				GiveCardForce(NewCard, ExceedDistance, Forces);
			}

			for (auto GetForceCard : AllCards)
			{
				const auto& GetForceCardPosition = Positions[GetForceCard];
				if (LocalContextSystem->IsOverlapExecuteArea(GetForceCardPosition))
				{
					// 给与斥力
					FVector Delta = GetForceCardPosition - LocalContextSystem->ExecuteArea->GetActorLocation();
					GiveCardForce(GetForceCard, Delta, Forces);
				}
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
								RedirectXForce2Y(CardForce);
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
											RedirectXForce2Y(CardForce);
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
								RedirectYForce2X(CardForce);
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
											RedirectYForce2X(CardForce);
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
				// 检查卡牌是不是在BlockArea内
				if (LocalContextSystem->IsOverlapExecuteArea(ACardPosition))
				{
					AllCardsNotOverlap = false;
					break;
				}
				
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
			UE_LOG(LogTemp, Warning, TEXT("FrameRecords.Append, CurrentLength: %d, AppendLength: %d"), FrameRecords.Num(), TaskResult.Num());
			FrameRecords.Append(TaskResult);
			UE_LOG(LogTemp, Warning, TEXT("FrameRecords.Append, AfterAppendLength: %d"), FrameRecords.Num());
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
	
	// 处理单边界情况，重定向力
	if (!FixedCards.Contains(InGetForceCard))
	{
		if (WillAtLeftBound && CurrentForce.Y < 0)
		{
			// 将Y方向的力重定向到X方向
			RedirectYForce2X(Forces[InGetForceCard]);
		}
		else if (WillAtRightBound && CurrentForce.Y > 0)
		{
			RedirectYForce2X(Forces[InGetForceCard]);
		}
		else if (WillAtTopBound && CurrentForce.X > 0)
		{
			RedirectXForce2Y(Forces[InGetForceCard]);
		}
		else if (WillAtBottomBound && CurrentForce.X < 0)
		{
			RedirectXForce2Y(Forces[InGetForceCard]);
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

void AProjectJCardLayoutManager::RedirectXForce2Y(FVector& WriteForce)
{
	auto RedirectForce = FMath::Abs(WriteForce.X);
	WriteForce.Y += WriteForce.Y > 0 ? RedirectForce : -RedirectForce;
	WriteForce.X = 0.0f;
}

void AProjectJCardLayoutManager::RedirectYForce2X(FVector& WriteForce)
{
	auto RedirectForce = FMath::Abs(WriteForce.Y);
	WriteForce.X += WriteForce.X > 0 ? RedirectForce : -RedirectForce;
	WriteForce.Y = 0.0f;
}

bool AProjectJCardLayoutManager::IsPositionInBounds(const FVector& Position, const FVector2D& DeskBounds)
{
	return Position.X >= -DeskBounds.X && Position.X <= DeskBounds.X &&
		   Position.Y >= -DeskBounds.Y && Position.Y <= DeskBounds.Y;
}

bool AProjectJCardLayoutManager::IsTwoCardOverlap(const FVector& ACardLocation, const FVector& BCardLocation,
	const FVector& CardSize)
{
	return FMath::Abs(ACardLocation.X - BCardLocation.X) < CardSize.X &&
			FMath::Abs(ACardLocation.Y - BCardLocation.Y) < CardSize.Y;
}
