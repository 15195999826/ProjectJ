// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialUI/ProjectJBigMap.h"

#include "Components/CanvasPanel.h"
#include "Core/DeveloperSettings/ProjectJDataTableSettings.h"
#include "Core/DeveloperSettings/ProjectJGeneralSettings.h"
#include "Core/System/ProjectJContextSystem.h"
#include "Core/System/ProjectJEventSystem.h"
#include "Game/ProjectJGameContext.h"
#include "Types/ProjectJDungeonConfig.h"
#include "Types/ProjectJMapPoint.h"
#include "UI/SpecialUI/ProjectJBigMapPoint.h"
#include "UI/SpecialUI/ProjectJAreaMap.h"
#include "UI/SpecialUI/ProjectJAreaMapPoint.h"

void UProjectJBigMap::BuildMapData()
{
#if WITH_EDITOR
	// 读取MapPointRoot下的所有BigMapPoint, 读取其配置
	auto Children = MapPointRoot->GetAllChildren();
	TArray<FProjectJMapPoint> MapPoints;
	for (auto Child : Children)
	{
		auto BigMapPoint = Cast<UProjectJBigMapPoint>(Child);
		if (BigMapPoint)
		{
			FProjectJMapPoint MapPoint;
			MapPoint.Name = FName(*BigMapPoint->PointText.ToString());
			auto TempAreaMap = CreateWidget<UProjectJAreaMap>(this, BigMapPoint->AreaMapClass);
			auto AreaMapPoints = TempAreaMap->MapPointRoot->GetAllChildren();
			for (auto AreaMapPoint : AreaMapPoints)
			{
				auto Point = Cast<UProjectJAreaMapPoint>(AreaMapPoint);
				if (Point)
				{
					MapPoint.AreaPointNames.Add(FName(*Point->PointText.ToString()));
				}
			}
			
			TempAreaMap->RemoveFromParent();
			TempAreaMap->ConditionalBeginDestroy();
			TempAreaMap = nullptr;

			MapPoints.Add(MapPoint);
		}
	}
	auto GSettings = GetMutableDefault<UProjectJGeneralSettings>();
	GSettings->MapPoints = MapPoints;
	FString DefaultConfigFilePath  = GSettings->GetDefaultConfigFilename();
	GSettings->SaveConfig(CPF_Config, *DefaultConfigFilePath);
#endif
}

TArray<FProjectJDungeonSimpleInfo> UProjectJBigMap::GetDungeonsAtAreaPoint(const FName& AreaPointName) const
{
	auto GameContext = GetWorld()->GetSubsystem<UProjectJContextSystem>()->GameContext;
	auto DungeonsTable = GetDefault<UProjectJDataTableSettings>()->DungeonTable.LoadSynchronous();
	TArray<FProjectJDungeonSimpleInfo> DungeonInfos;
	for (const auto& PossibleDungeonLocation : GameContext->PossibleDungeonLocations)
	{
		if (PossibleDungeonLocation.Value.AreaPoint == AreaPointName)
		{
			auto DungeonConfig = DungeonsTable->FindRow<FProjectJDungeonConfig>(PossibleDungeonLocation.Key, TEXT("UProjectJBigMap::GetDungeonsAtAreaPoint"));
			check(DungeonConfig);
			FProjectJDungeonSimpleInfo DungeonInfo;
			DungeonInfo.RowName = PossibleDungeonLocation.Key;
			DungeonInfo.DisplayText = DungeonConfig->Name;
			DungeonInfos.Add(DungeonInfo);
		}
	}

	return DungeonInfos;
}

void UProjectJBigMap::NativeConstruct()
{
	Super::NativeConstruct();

	auto MapPoints = MapPointRoot->GetAllChildren();
	for (auto MapPoint : MapPoints)
	{
		auto BigMapPoint = Cast<UProjectJBigMapPoint>(MapPoint);
		if (BigMapPoint)
		{
			// Todo: 这里如果是本地化了，是不是会读取错误？
			auto PointName =  FName(BigMapPoint->PointText.ToString());
			MapPointCache.Add(PointName, BigMapPoint);
		}
	}
	
	auto EventSystem = GetWorld()->GetSubsystem<UProjectJEventSystem>();
	EventSystem->OnDungeonsRefresh.AddUObject(this, &UProjectJBigMap::OnDungeonsRefresh);
}

void UProjectJBigMap::OnDungeonsRefresh()
{
	for (const auto Point : MapPointCache)
	{
		Point.Value->HighLightCircle(false);
	}
	
	auto MapConfig = GetDefault<UProjectJGeneralSettings>()->MapPoints;
	auto GameContext = GetWorld()->GetSubsystem<UProjectJContextSystem>()->GameContext;
	for (const auto& PossibleDungeon : GameContext->PossibleDungeons)
	{
		auto DungeonLocation = GameContext->PossibleDungeonLocations[PossibleDungeon];
		auto BigMapPoint = MapPointCache[DungeonLocation.ParentPoint];
		BigMapPoint->HighLightCircle(true);
	}
}
