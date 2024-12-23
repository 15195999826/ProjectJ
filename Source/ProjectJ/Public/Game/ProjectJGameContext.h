// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/ProjectJDateTime.h"
#include "ProjectJGameContext.generated.h"

UCLASS()
class PROJECTJ_API AProjectJGameContext : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectJGameContext();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY()
	FName CurrentLevel = NAME_None;
	UPROPERTY()
	FProjectJDateTime DateTime;
	UPROPERTY()
	TArray<int32> LevelCharacters;
	UPROPERTY()
	TArray<int32> LevelLandmarks;
	UPROPERTY()
	TArray<int32> LevelNavPoints;
	UPROPERTY()
	TArray<int32> HandSpellCards;

	// 用于计算是否首次进入某个关卡
	TArray<FName> HasEnteredLevels;
};
