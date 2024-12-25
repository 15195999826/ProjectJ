// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildLevel/ProjectJBuildLevelPC.h"
#include "GameFramework/Actor.h"
#include "Interface/ProjectJDragInterface.h"
#include "Types/CardType.h"
#include "ProjectJCardBase.generated.h"

class UTextRenderComponent;
class UPaperSpriteComponent;
class UProjectJContextSystem;
class UBoxComponent;

UCLASS(Abstract)
class PROJECTJ_API AProjectJCardBase : public AActor, public IProjectJDragInterface
{
	GENERATED_BODY()

	friend UProjectJContextSystem;
public:	
	// Sets default values for this actor's properties
	AProjectJCardBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> Collider;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPaperSpriteComponent> FrameSprite;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UTextRenderComponent> Title;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ID;
	bool CanDrag = true;
	bool CanSelect = false;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(CallInEditor)
	void PrintMeshSize();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
protected:
	UPROPERTY()
	FName ConfigRowName;
	
	FVector DragStartLocation;

public:
	// Start IProjectJDragInterface
	virtual bool IsDraggable() override
	{
		return CanDrag;
	}
	virtual void OnDragStart() override;
	virtual void OnDragTick(float DeltaSeconds, const FVector& ToLocation, float InBaseRotationX,float InMaxShakeDelta, float InLerpSpeed) override;
	
	virtual void OnDrop(float InDuration) override;
	virtual void OnCancelDrag() override;

	virtual FVector GetCurrentLocation() override;
	// End IProjectJDragInterface

	// Todo: 不同类型的卡牌可以有不同的等待选择效果
	virtual void OnWaitForExecuteSelect();
	virtual void EndExecuteSelect();
	
	//----- 程序动画 Start -------
	// 公开接口
	UFUNCTION(BlueprintCallable)
	void HideCard(float Duration = 0.5f);
    
	UFUNCTION(BlueprintCallable) 
	void ShowCard(float Duration = 0.5f);
    
	UFUNCTION(BlueprintCallable)
	void PopupCard(const FVector& StartLocation, const FVector& TargetLocation, float Duration = 0.8f);

	void PerformSelected();
private:
	// 1. 拖拽卡牌
	FTimerHandle DropOnGroundTimerHandle;
	float DropOnGroundStartTime;
	float DropOnGroundDuration = 0.5f;
	FVector DropOnGroundStartLocation;
	bool IsDropOnExecuteArea = false;
	void UpdateDropOnGroundAnimation();

	// 通用动画数据
	FTimerHandle CardAnimationTimerHandle;
	float AnimationStartTime;
	float AnimationDuration;
	// 2. 隐藏卡牌
	void PlayHideAnimation(float Duration = 0.5f);
	void UpdateHideAnimation();
	FVector InitialScale;

	// 3. 卡牌出现
	void PlayShowAnimation(float Duration = 0.5f);
	void UpdateShowAnimation();

	// 4. 卡牌从某个位置旋转弹出
	void PlayPopupAnimation(const FVector& StartLocation, const FVector& TargetLocation, float Duration = 0.8f);
	void UpdatePopupAnimation();
	FVector PopupStartLocation;
	FVector PopupTargetLocation;
	FRotator PopupStartRotation;
	FRotator PopupTargetRotation;
	
	// 5. 卡牌被点击选中


	// ---- 程序动画 End ------

	// ---- 放置卡牌 辅助函数 Start ----
	
	// 计算卡牌之间所需的最小移动距离
	float CalculateRequiredDistance(const FVector2D& CardSize, const FVector& Direction) const;

	

	// 处理特殊边界情况
	bool HandleBorderCase(const FVector& OtherCardPosition, const FVector& SelfDesiredLocation,
	                      const FVector& OtherCardDstLocation, const FVector& CardSize, const FVector2D& DeskBounds,
	                      FVector& OutDstPosition) const;
	// 确定旋转方向
	bool DetermineRotateClockwise(const FVector& OtherCardPosition, const FVector& SelfDesiredLocation, const FVector& OtherCardDstLocation,const FVector2D& DeskBounds);
	// 通过旋转查找有效位置
	bool FindValidPositionByRotation(const FVector& SelfDesiredLocation, const FVector& MoveDirection,
	                                 const FVector2D& CardSize, const FVector2D& DeskBounds, bool RotateClockwise,
	                                 FVector& OutNewLocation) const;

	// ---- 放置卡牌 辅助函数 End ----
};