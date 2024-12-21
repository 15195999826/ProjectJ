// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/ProjectJCardAnimState.h"
#include "ProjectJLevelSettingActor.generated.h"

USTRUCT(BlueprintType)
struct FProjectJBattleAreaSettings
{
	GENERATED_BODY()
	
	FProjectJBattleAreaSettings(): TeamOffsetY(0)
	{
	}

	UPROPERTY(EditAnywhere)
	FVector TopTeamCenterPos = FVector(200, 0 ,0);

	UPROPERTY(EditAnywhere)
	FVector BottomTeamCenterPos = FVector(-200, 0 ,0);

	UPROPERTY(EditAnywhere)
	float TeamOffsetY = 200.f;
};

USTRUCT(BlueprintType)
struct FCardHideAnimConfig
{
	GENERATED_BODY()

	// 总动画持续时间
	UPROPERTY(EditAnywhere, Category="Hide Animation", meta=(ClampMin="0.1", DisplayName="总动画持续时间"))
	float Duration = 0.5f;

	// 放大阶段时间占比
	UPROPERTY(EditAnywhere, Category="Hide Animation", meta=(ClampMin="0.0", ClampMax="1.0", DisplayName="放大阶段时间占比"))
	float ScaleUpPhaseRatio = 0.3f;

	// 最大放大比例
	UPROPERTY(EditAnywhere, Category="Hide Animation", meta=(ClampMin="1.0", DisplayName="最大放大比例"))
	float MaxScaleFactor = 1.2f;
};

USTRUCT(BlueprintType)
struct FCardShowAnimConfig
{
	GENERATED_BODY()

	// 总动画持续时间
	UPROPERTY(EditAnywhere, Category="Show Animation", meta=(ClampMin="0.1", DisplayName="总动画持续时间"))
	float Duration = 0.5f;

	// 放大阶段时间占比
	UPROPERTY(EditAnywhere, Category="Show Animation", meta=(ClampMin="0.0", ClampMax="1.0", DisplayName="放大阶段时间占比"))
	float ScaleUpPhaseRatio = 0.6f;

	// 最大放大比例
	UPROPERTY(EditAnywhere, Category="Show Animation", meta=(ClampMin="1.0", DisplayName="最大放大比例"))
	float MaxScaleFactor = 1.2f;
};

USTRUCT(BlueprintType)
struct FCardRotateSpawnAnimConfig
{
	GENERATED_BODY()

	// 总动画持续时间
	UPROPERTY(EditAnywhere, Category="Rotate Spawn Animation", meta=(ClampMin="0.1",DisplayName="总动画持续时间"))
	float Duration = 0.8f;

	// 最大旋转角度
	UPROPERTY(EditAnywhere, Category="Rotate Spawn Animation", meta=(ClampMin="0.0", ClampMax="720.0", DisplayName="最大旋转角度"))
	float MaxRotationDegrees = 720.0f;

	// 抛物线最大高度
	UPROPERTY(EditAnywhere, Category="Rotate Spawn Animation", meta=(ClampMin="100.0", DisplayName="抛物线最大高度"))
	float MaxArcHeight = 200.0f;
};

UCLASS()
class PROJECTJ_API AProjectJLevelSettingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectJLevelSettingActor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Config, meta=(DisplayName="是否使用力导向算法布局"))
	bool UseCardLayoutManager = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Config)
	FVector HandSpellCardStartLocation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Config)
	FVector HandSpellCardOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Config)
	FVector TalkWidgetOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Config)
	FProjectJProgramAnimationSettings ProgramAnimationSettings;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Config)
	FProjectJBattleAreaSettings BattleAreaSettings;

	UPROPERTY(config, EditAnywhere, Category = "Card Animations")
	FCardHideAnimConfig HideAnimConfig;

	UPROPERTY(config, EditAnywhere, Category = "Card Animations")
	FCardShowAnimConfig ShowAnimConfig;

	UPROPERTY(config, EditAnywhere, Category = "Card Animations")
	FCardRotateSpawnAnimConfig PopSpawnAnimConfig;
};
