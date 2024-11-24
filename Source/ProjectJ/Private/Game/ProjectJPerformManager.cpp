// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ProjectJPerformManager.h"

#include "Core/System/ProjectJEventSystem.h"

// Sets default values
AProjectJPerformManager::AProjectJPerformManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AProjectJPerformManager::BeginPlay()
{
	Super::BeginPlay();
	auto EventSystem = GetWorld()->GetSubsystem<UProjectJEventSystem>();
	EventSystem->PostStoryRecord.AddDynamic(this, &AProjectJPerformManager::OnPostStoryRecord);
}

// Called every frame
void AProjectJPerformManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsRunPerform)
	{
		if  (PerformQueue.Num() > 0)
		{
			if (IsPerforming)
			{
				return;
			}

			const auto PerformType = PerformQueue[0];
			PerformQueue.RemoveAt(0);
			switch (PerformType) {
				case EProjectJPerformType::None:
					break;
				case EProjectJPerformType::Story:
					{
						IsPerforming = true;
						const auto& StoryRecord = StoryRecords[0];
						PerformStory(StoryRecord);
					}
					break;
			}
		}
		else
		{
			if (!IsPerforming)
			{
				IsRunPerform = false;
				auto EventSystem = GetWorld()->GetSubsystem<UProjectJEventSystem>();
				EventSystem->OnPerformEnd.Broadcast();
			}
		}
	}
}

void AProjectJPerformManager::StartPerform()
{
	IsRunPerform = true;
}

void AProjectJPerformManager::OnPostStoryRecord(const FProjectJStoryRecord& ProjectJStoryRecord)
{
	StoryRecords.Add(ProjectJStoryRecord);
	PerformQueue.Add(EProjectJPerformType::Story);
}

void AProjectJPerformManager::PerformStoryEnd_Implementation()
{
	IsPerforming = false;
	StoryRecords.RemoveAt(0);
}

void AProjectJPerformManager::PerformStory_Implementation(const FProjectJStoryRecord& InStoryRecord)
{
}

