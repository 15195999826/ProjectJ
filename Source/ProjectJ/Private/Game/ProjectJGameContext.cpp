// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ProjectJGameContext.h"

#include "Core/DeveloperSettings/ProjectJDataTableSettings.h"
#include "Core/System/ProjectJEventSystem.h"
#include "Types/ProjectJDungeonConfig.h"

// Sets default values
AProjectJGameContext::AProjectJGameContext(): MainCharacterID(0)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AProjectJGameContext::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProjectJGameContext::RefreshDungeons()
{
	PossibleDungeons.Empty();
	PossibleDungeonLocations.Empty();
	
	auto DungeonTable = GetDefault<UProjectJDataTableSettings>()->DungeonTable.LoadSynchronous();

	auto RowMap = DungeonTable->GetRowMap();
	for (auto& Pair : RowMap)
	{
		auto DungeonConfig = reinterpret_cast<FProjectJDungeonConfig*>(Pair.Value);
		// Todo: 目前只考虑时间刷新
		if (DungeonConfig->SpawnCondition.Day == DateTime.Day)
		{
			PossibleDungeons.Add(Pair.Key);
			auto PointStr = DungeonConfig->Point.ToString();
			// Split By '_'
			TArray<FString> PointStrArray;
			PointStr.ParseIntoArray(PointStrArray, TEXT("_"), true);
			check(PointStrArray.Num() == 2);
			FProjectJDungeonLocation Location;
			Location.ParentPoint = FName(*PointStrArray[0]);
			Location.AreaPoint = FName(*PointStrArray[1]);
			PossibleDungeonLocations.Add(Pair.Key, Location);
		}
	}
	
	auto EventSystem = GetWorld()->GetSubsystem<UProjectJEventSystem>();
	EventSystem->OnDungeonsRefresh.Broadcast();
}
