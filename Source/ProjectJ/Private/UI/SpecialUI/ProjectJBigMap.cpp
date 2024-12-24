// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpecialUI/ProjectJBigMap.h"

#include "Components/CanvasPanel.h"
#include "Core/DeveloperSettings/ProjectJGeneralSettings.h"
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
