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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="卡牌间固定斥力"))
	float BaseRepulsionForce = 1000.0f;
	// 最大迭代次数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="最大迭代次数"))
	int32 MaxIterations = 50;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="每次受力迭代时间间隔"))
	float IterationInterval = 0.01f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="力的重定向系数"))
	float ForceRedirectFactor = 1.f;
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
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Layout")
	FLayoutConfig LayoutConfig;

	/**
	 * 
	 * @param NewCard
	 * @param InDesiredDropDuration 
	 * @param NewCardDesiredLocation
	 * @return 返回是否需要发生调整
	 */
	bool PlaceCardAndRelayout(class AProjectJCardBase* NewCard,
	                          float InDesiredDropDuration,
	                          const FVector& NewCardDesiredLocation);

	void OnDragCard(int32 InCardID);
	void OnStopDragCard(int32 InCardID);

	bool RandomEmptyLocation(FVector& OutLocation);
private:
	int32 DraggingCardID = INT_MIN;
	bool IsControlCardDrop = false;
	int32 DroppingCardID = -1;
	FVector DropOnGroundStartLocation;
	float DroppingStartTime = 0.0f;
	float DesiredDropDuration = 0.0f;
	TArray<TMap<int32, FVector>> FrameRecords;

	void HandleBoundaryForces(AProjectJCardBase* InGetForceCard, 
						 const FVector& InGetForceCardPosition,
						 const FVector2D& DeskBounds,
						 TMap<AProjectJCardBase*, FVector>& Forces,
						 TSet<AProjectJCardBase*>& FixedCards);

	/**
     * 禁止进入一个区域
     */
	
	
	void GiveCardForce(AProjectJCardBase* InCard, FVector& InDelta, TMap<AProjectJCardBase*, FVector>& Forces);
	
	void RedirectXForce2Y(FVector& WriteForce);
	void RedirectYForce2X(FVector& WriteForce);

public:
	// 检查位置是否在桌面边界内
	static bool IsPositionInBounds(const FVector& Position, const FVector2D& DeskBounds);
	// 卡牌是否重叠
	static bool IsTwoCardOverlap(const FVector& ACardLocation, const FVector& BCardLocation, const FVector& CardSize);

};
