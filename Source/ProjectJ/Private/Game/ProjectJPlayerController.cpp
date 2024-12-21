// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ProjectJPlayerController.h"

#include "Core/System/ProjectJContextSystem.h"
#include "Core/System/ProjectJEventSystem.h"
#include "Game/Card/ProjectJCardExecuteArea.h"
#include "Game/Card/ProjectJSpell.h"


void AProjectJPlayerController::BeginPlay()
{
	Super::BeginPlay();
	auto EventSystem = GetWorld()->GetSubsystem<UProjectJEventSystem>();
	EventSystem->OnStageChange.AddUObject(this, &AProjectJPlayerController::OnStageChange);
}

void AProjectJPlayerController::CustomTick(float DeltaSeconds, bool OverWidget, bool IsHitGround,
                                           const FVector& HitGroundLocation, AActor* InHitActor)
{
	if (IsPending)
	{
		return;
	}
	
	if (RightMouseState == EProjectJMouseState::Press)
	{
		if (IsDragging)
		{
			OnCancelDrag();
		}
		else
		{
			if (InHitActor)
			{
				// Todo: 右键点击卡牌功能
			}
		}
		return;
	}
	
	switch (LeftMouseState) {
		case EProjectJMouseState::Invalid:
			break;
		case EProjectJMouseState::Idle:
			break;
		case EProjectJMouseState::Press:
			{
				if (InHitActor && InHitActor->Implements<UProjectJDragInterface>())
				{
					auto DragInterface = Cast<IProjectJDragInterface>(InHitActor);
					if (DragInterface->IsDraggable())
					{
						// Todo 是否要把其它手牌设置为无碰撞状态
						OnDragStart(InHitActor);
					}
				}
			}
		break;
		case EProjectJMouseState::Pressing:
			{
				if (IsDragging)
				{
					auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
					if (ContextSystem->IsInExecuteArea(DragActor->GetCurrentLocation()))
					{
						// Todo: 根据卡牌能否执行显示不同的效果
						ContextSystem->ExecuteArea->OnFocus();
					}
					else
					{
						ContextSystem->ExecuteArea->OnLoseFocus();
					}
					
					// Todo: 都是临时写法
					// if (InHitActor && InHitActor->Implements<UProjectJCardInterface>())
					// {
					// 	auto CardType = IProjectJCardInterface::Execute_GetCardType(InHitActor);
					// 	if (CardType != EProjectJCardType::Spell)
					// 	{
					// 		if (ContextSystem->WeakFocusCard.IsValid() && ContextSystem->WeakFocusCard != InHitActor)
					// 		{
					// 			ContextSystem->WeakFocusCard->OnLoseSpellFocus();
					// 		}
					// 		ContextSystem->WeakFocusCard = Cast<AProjectJCardBase>(InHitActor);
					// 		ContextSystem->WeakFocusCard->OnSpellFocus();
					// 	}
					// }
					// else
					// {
					// 	if (ContextSystem->WeakFocusCard.IsValid())
					// 	{
					// 		ContextSystem->WeakFocusCard->OnLoseSpellFocus();
					// 		ContextSystem->WeakFocusCard = nullptr;
					// 	}
					// }
				}
			}
			break;
		case EProjectJMouseState::Release:
			{
				if (IsDragging)
				{
					OnDrop();
				}
			}
		break;
	}
	
	if (IsDragging)
	{
		// 输入鼠标位置
		FVector WorldLocation;
		FVector WorldDirection;
		bool Success = DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
		if (Success)
		{
			// 从WorldLocation沿着WorldDirection方向移动，直到Z值为100
			FVector NewLocation = WorldLocation + WorldDirection * ((100.f - WorldLocation.Z) / WorldDirection.Z);
			NewLocation.Z = 100.f;
			// DrawDebugSphere(GetWorld(), DrawLocation, 3.f, 12, FColor::Red, false, 0.1f);	
			DragActor->OnDragTick(DeltaSeconds, NewLocation, BaseRotationX, MaxShakeDelta, LerpSpeed);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("DeprojectMousePositionToWorld failed!"));
		}
	}
}

void AProjectJPlayerController::OnDragStart(AActor* InDragActor)
{
	check(IsDragging == false);
	IsDragging = true;
	DragActor = InDragActor;
	DragActor->OnDragStart();
}

void AProjectJPlayerController::OnDrop()
{
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	ContextSystem->ExecuteArea->OnLoseFocus();
	IsDragging = false;
	// 当前卡牌放置到鼠标命中地面的位置
	DragActor->OnDrop(DropOnGroundDuration);
	DragActor = nullptr;
}

void AProjectJPlayerController::OnCancelDrag()
{
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	ContextSystem->ExecuteArea->OnLoseFocus();
	// if (ContextSystem->WeakFocusCard.IsValid())
	// {
	// 	ContextSystem->WeakFocusCard->OnLoseSpellFocus();
	// 	ContextSystem->WeakFocusCard = nullptr;
	// }
	
	DragActor->OnCancelDrag();
	IsDragging = false;
	DragActor = nullptr;
}

void AProjectJPlayerController::OnStageChange(EProjectJGameStage OldStage, EProjectJGameStage NewStage)
{
	if (NewStage != EProjectJGameStage::Idle)
	{
		IsPending = true;
	}
	else
	{
		IsPending = false;
	}
}
