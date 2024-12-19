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
	// X方向(长)和Y方向(宽)的力量系数
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(DisplayName="X方向(长)和Y方向(宽)的力量系数(根据卡牌尺寸自动设置)"))
	float DirectionalForceRatio = 1.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="卡牌间基础排斥力"))
	float BaseRepulsionForce = 1000.0f;
	
	// 向目标位置的吸引力
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="向目标位置的吸引力"))
	float AttractionForce = 0.01f;
	// 边界排斥力
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="边界排斥力"))
	float BoundaryForce = 2000.0f;
	// 边界影响范围系数(相对于卡牌尺寸)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="边界影响范围系数(相对于卡牌尺寸)"))
	float BoundaryEffectRatio = 0.2f;
	// 最大迭代次数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="最大迭代次数"))
	int32 MaxIterations = 50;
	// 收敛阈值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="收敛阈值"))
	float ConvergenceThreshold = 1.0f;
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

	bool PlaceCardAndRelayout(class AProjectJCardBase* NewCard, 
						  const FVector& DesiredLocation,
						  const FVector2D& DeskBounds,
						  const FVector2D& CardSize);

private:
	TMap<AProjectJCardBase*, FVector> CalculateOptimalLayout(
	   const TArray<AProjectJCardBase*>& AllCards,
	   AProjectJCardBase* NewCard,
	   const FVector& DesiredLocation,
	   const FVector2D& DeskBounds,
	   const FVector2D& CardSize,
	   const FLayoutConfig& Config);

	TArray<FTimerHandle> MoveTimers;
};
