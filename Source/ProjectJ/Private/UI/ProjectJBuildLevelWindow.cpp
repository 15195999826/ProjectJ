// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ProjectJBuildLevelWindow.h"

#include "EngineUtils.h"
#include "Components/TextBlock.h"
#include "Core/DeveloperSettings/ProjectJDataTableSettings.h"
#include "Core/System/ProjectJEventSystem.h"
#include "Game/ProjectJNavPointActor.h"
#include "Game/Card/ProjectJCardBase.h"
#include "Interface/ProjectJCardInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectJ/ProjectJLogChannels.h"

void UProjectJBuildLevelWindow::CreateNewLevel(const FName& InLevelName)
{
	auto EventSystem = GetWorld()->GetSubsystem<UProjectJEventSystem>();
	EventSystem->OnCreateLevel.Broadcast(InLevelName);
}

void UProjectJBuildLevelWindow::CreateNewCharacter(const FName& InCharacterName)
{
	auto EventSystem = GetWorld()->GetSubsystem<UProjectJEventSystem>();
	EventSystem->OnCreateCharacter.Broadcast(InCharacterName);
}

void UProjectJBuildLevelWindow::CreateNewLandmark(const FName& InLandmarkName)
{
	auto EventSystem = GetWorld()->GetSubsystem<UProjectJEventSystem>();
	EventSystem->OnCreateLandmark.Broadcast(InLandmarkName);
}

void UProjectJBuildLevelWindow::SaveLevel()
{
	if (EditLevelRowName == NAME_None)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("未选择关卡"), true, true, FLinearColor::Red, 2.f);
		return;
	}

	auto DTSettings = GetDefault<UProjectJDataTableSettings>();
	auto LevelRows = DTSettings->LevelTable.LoadSynchronous()->GetRowNames();
	auto CharacterRows = DTSettings->CharacterTable.LoadSynchronous()->GetRowNames();
	auto LandmarkRows = DTSettings->LandmarkTable.LoadSynchronous()->GetRowNames();
	FProjectJLevelConfig LevelConfig;
	// 获取全部的，NavPointActor\Character\Landmark
	for (TActorIterator<AProjectJNavPointActor> It(GetWorld()); It; ++It)
	{
		auto NavPointActor = *It;
		if (LevelRows.Contains(NavPointActor->ToLevel) == false)
		{
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("不存在的关卡: %s, 导航点未保存"), *NavPointActor->ToLevel.ToString()), true, true, FLinearColor::Red, 2.0f);
			continue;
		}
		
		FProjectJNavPoint NavPoint;
		NavPoint.ToLevel = NavPointActor->ToLevel;
		NavPoint.Location = NavPointActor->GetActorLocation();
		LevelConfig.NavPoints.Add(NavPoint);
	}

	for (TActorIterator<AProjectJCardBase> It(GetWorld()); It; ++It)
	{
		auto CardActor = *It;
		auto CardType = IProjectJCardInterface::Execute_GetCardType(CardActor);
		switch (CardType) {
			case EProjectJCardType::Character:
				{
					if (CharacterRows.Contains(IProjectJCardInterface::Execute_GetConfigRowName(CardActor)) == false)
					{
						UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("不存在的角色: %s, 角色未保存"), *IProjectJCardInterface::Execute_GetConfigRowName(CardActor).ToString()), true, true, FLinearColor::Red, 2.0f);
						continue;
					}
					FProjectJLevelCardPoint CardPoint;
					CardPoint.Location = CardActor->GetActorLocation();
					CardPoint.RowName = IProjectJCardInterface::Execute_GetConfigRowName(CardActor);
					LevelConfig.Characters.Add(CardPoint);
				}
				break;
			case EProjectJCardType::Landmark:
				{
					if (LandmarkRows.Contains(IProjectJCardInterface::Execute_GetConfigRowName(CardActor)) == false)
					{
						UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("不存在的地标: %s, 地标未保存"), *IProjectJCardInterface::Execute_GetConfigRowName(CardActor).ToString()), true, true, FLinearColor::Red, 2.0f);
						continue;
					}
					FProjectJLevelCardPoint CardPoint;
					CardPoint.Location = CardActor->GetActorLocation();
					CardPoint.RowName = IProjectJCardInterface::Execute_GetConfigRowName(CardActor);
					LevelConfig.Landmarks.Add(CardPoint);
				}
				break;
			default:
				UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("不支持保存在关卡中的类型: %d"), (int32)CardType), true, true, FLinearColor::Red, 2.0f);
				break;
		}
	}

	auto EventSystem = GetWorld()->GetSubsystem<UProjectJEventSystem>();
	EventSystem->OnSaveLevel.Broadcast(EditLevelRowName, LevelConfig);
}

void UProjectJBuildLevelWindow::LoadLevel_Implementation(FName InLevelRowName)
{
	EditLevelRowName = InLevelRowName;
	EditLevelText->SetText(FText::FromName(InLevelRowName));
}
