// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildLevel/ProjectJBuildLevelPC.h"
#include "Interface/ProjectJDragInterface.h"

void AProjectJBuildLevelPC::CustomTick(float DeltaSeconds, bool OverWidget, bool IsHitGround,
                                       const FVector& HitGroundLocation, AActor* InHitActor)
{
	// 如果右键按下并且正在拖拽卡牌， 卡牌回归原位
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
				EditCard(InHitActor);
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
					OnDragStart(InHitActor);
				}
			}
		break;
		case EProjectJMouseState::Pressing:
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

void AProjectJBuildLevelPC::OnDragStart(AActor* InDragActor)
{
	check(IsDragging == false);
	IsDragging = true;
	DragActor = InDragActor;
	DragActor->OnDragStart();
}

void AProjectJBuildLevelPC::OnDrop()
{
	IsDragging = false;
	// 当前卡牌放置到鼠标命中地面的位置
	DragActor->OnDrop(DropOnGroundDuration);
	DragActor = nullptr;
}

void AProjectJBuildLevelPC::OnCancelDrag()
{
	DragActor->OnCancelDrag();
	IsDragging = false;
	DragActor = nullptr;
}


