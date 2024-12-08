// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Types/ProjectJLevelConfig.h"
#include "Types/ProjectJLuaInstanceType.h"
#include "BuildLevelWorldSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTJEDITOR_API UBuildLevelWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
protected:
	void OnLevelPrepared();
	
	void CreateNewLevel(const FName& InLevelName);
	
	void CreateNewCharacter(const FName& InCharacterName);

	void CreateNewLandmark(const FName& InLandmarkName);
	
	void SaveLevel(const FName& Name, const FProjectJLevelConfig& ProjectJLevelConfig);
	
};
