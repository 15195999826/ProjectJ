// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/ProjectJDateTime.h"
#include "ProjectJGameContext.generated.h"

class AProjectJCardBase;

USTRUCT()
struct FProjectJDungeonLocation
{
	GENERATED_BODY()
	
	UPROPERTY()
	FName ParentPoint;
	UPROPERTY()
	FName AreaPoint;
};

/**
 * 仅用于存储游戏数据
 */
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
	int32 MainCharacterID;
	int32 UsingCharacterID;
	
	UPROPERTY()
	FProjectJDateTime DateTime;
	
	UPROPERTY()
	FName CurrentDungeon = NAME_None;

	UPROPERTY()
	TArray<FName> PossibleDungeons;
	UPROPERTY()
	TMap<FName, FProjectJDungeonLocation> PossibleDungeonLocations;
	
	UPROPERTY()
	TArray<int32> LevelCharacters;
	UPROPERTY()
	TArray<int32> LevelLandmarks;
	UPROPERTY()
	TArray<int32> LevelUtilities;

	UPROPERTY()
	TArray<TObjectPtr<AProjectJCardBase>> InDungeonCards;
	UPROPERTY()
	TMap<int32, TObjectPtr<AProjectJCardBase>> InDungeonCardsMap;
	
public:
	void RefreshDungeons();
};
