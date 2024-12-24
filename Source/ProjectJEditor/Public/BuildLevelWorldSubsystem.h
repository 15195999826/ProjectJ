// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
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
	
	void CreateNewCharacter(const FName& InCharacterName);

	void CreateNewLandmark(const FName& InLandmarkName);

	void CreateNewUtility(const FName& InUtilityName);
	
	// void SaveLevel(const FName& Name, const FProjectJLevelConfig& ProjectJLevelConfig);
	
};
