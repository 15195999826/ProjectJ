// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectJCardLayoutManager.generated.h"

// 布局配置参数结构体
USTRUCT(BlueprintType)
struct FLayoutConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="卡牌间基础排斥力"))
	float BaseRepulsionForce = 1000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="边界排斥力"))
	float BoundaryForce = 2000.0f;

	// 最大迭代次数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="最大迭代次数"))
	int32 MaxIterations = 50;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="每次受力迭代时间间隔"))
	float IterationInterval = 0.01f;
};

UCLASS()
class PROJECTJ_API AProjectJCardLayoutManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectJCardLayoutManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Layout")
	FLayoutConfig LayoutConfig;

	/**
	 * 
	 * @param NewCard 
	 * @param NewCardDesiredLocation 
	 * @param DeskBounds 
	 * @param CardSize 
	 * @return 返回是否需要发生调整
	 */
	bool PlaceCardAndRelayout(class AProjectJCardBase* NewCard, 
	                          const FVector& NewCardDesiredLocation,
	                          const FVector2D& DeskBounds,
	                          const FVector2D& CardSize);

private:
	TArray<FTimerHandle> MoveTimers;

	TArray<TMap<AProjectJCardBase*, FVector>> DebugFrameStatus;
	int32 DebugCursor = 0;

	void ExecDebugFrameStatus();

	void HandleBoundaryForces(AProjectJCardBase* InGetForceCard, 
						 const FVector& InGetForceCardPosition,
						 const FVector2D& DeskBounds,
						 TMap<AProjectJCardBase*, FVector>& Forces,
						 TSet<AProjectJCardBase*>& FixedCards);

	void GiveCardForce(AProjectJCardBase* InCard, FVector& InDelta, TMap<AProjectJCardBase*, FVector>& Forces);

public:
	// 检查位置是否在桌面边界内
	static bool IsPositionInBounds(const FVector& Position, const FVector2D& DeskBounds);
	// 卡牌是否重叠
	static bool IsTwoCardOverlap(const FVector& ACardLocation, const FVector& BCardLocation, const FVector2D& CardSize);

};
