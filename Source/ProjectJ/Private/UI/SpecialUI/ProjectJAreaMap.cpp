// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialUI/ProjectJAreaMap.h"

#include "Components/CanvasPanel.h"
#include "Core/System/ProjectJContextSystem.h"
#include "Game/ProjectJGameContext.h"
#include "UI/SpecialUI/ProjectJAreaMapPoint.h"

void UProjectJAreaMap::UpdateDungeons()
{
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	auto MapPoints = MapPointRoot->GetAllChildren();
	for (auto& Point : MapPoints)
	{
		auto AreaPoint = Cast<UProjectJAreaMapPoint>(Point);
		auto AreaPointName = FName(*AreaPoint->PointText.ToString());
		for (const auto& Dungeon : ContextSystem->GameContext->PossibleDungeons)
		{
			auto DungeonLocation = ContextSystem->GameContext->PossibleDungeonLocations[Dungeon];
			if (DungeonLocation.AreaPoint == AreaPointName)
			{
				AreaPoint->HighLightCircle(true);
			}
		}
	}
}
