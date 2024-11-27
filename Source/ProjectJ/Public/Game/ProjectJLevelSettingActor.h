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

UCLASS()
class PROJECTJ_API AProjectJLevelSettingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectJLevelSettingActor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Config)
	FVector HandSpellCardStartLocation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Config)
	FVector HandSpellCardOffset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Config)
	FRotator HandSpellCardRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Config)
	FVector TalkWidgetOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Config)
	FProjectJProgramAnimationSettings ProgramAnimationSettings;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Config)
	FProjectJBattleAreaSettings BattleAreaSettings;
};
