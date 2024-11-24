// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/ProjectJRecord.h"
#include "ProjectJPerformManager.generated.h"

UCLASS()
class PROJECTJ_API AProjectJPerformManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectJPerformManager();

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	bool IsRunPerform = false;
	bool IsPerforming = false;
	
	TArray<EProjectJPerformType> PerformQueue;
	
	TArray<FProjectJStoryRecord> StoryRecords;
	
public:
	void StartPerform();

protected:
	UFUNCTION(BlueprintNativeEvent)
	void PerformStory(const FProjectJStoryRecord& InStoryRecord);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PerformStoryEnd();


private:
	UFUNCTION()
	void OnPostStoryRecord(const FProjectJStoryRecord& ProjectJStoryRecord);
};
