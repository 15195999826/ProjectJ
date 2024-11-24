// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildLevelWorldSubsystem.h"

#include "DataTableEditorUtils.h"
#include "FileHelpers.h"
#include "IPythonScriptPlugin.h"
#include "PythonBridge.h"
#include "BuildLevel/ProjectJBuildLevelGameMode.h"
#include "Game/ProjectJGameMode.h"
#include "Core/DeveloperSettings/ProjectJDataTableSettings.h"
#include "Core/System/ProjectJEventSystem.h"
#include "Types/ProjectJCharacterConfig.h"
#include "Types/ProjectJLandmarkConfig.h"
#include "Types/ProjectJLevelConfig.h"


void UBuildLevelWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	auto GameMode = InWorld.GetAuthGameMode();

	if (GameMode == nullptr)
	{
		return;
	}

	auto ProjectJGameMode = Cast<AProjectJBuildLevelGameMode>(GameMode);
	if (ProjectJGameMode)
	{
		ProjectJGameMode->OnLevelPrepared.AddUObject(this, &UBuildLevelWorldSubsystem::OnLevelPrepared);
	}
	UPythonBridge* bridge = UPythonBridge::Get();
	// bridge->FunctionImplementedInPython();
	auto Ret = bridge->ToPinyin(TEXT("你好，世界"));
	UE_LOG(LogTemp, Warning, TEXT("UPythonBridge::ToPinyin() = %s"), *Ret);
	
	// FString ScriptPath = FPaths::ProjectDir() / TEXT("PythonScript/ToPinyin.py");
	// FString PythonCode;
	// if (FFileHelper::LoadFileToString(PythonCode, *ScriptPath))
	// {
	// 	IPythonScriptPlugin::Get()->ExecPythonCommand(*PythonCode);
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("Failed to load Python script: %s"), *ScriptPath);
	// }

	// FString PythonCode = TEXT("print('Hello from Python in Unreal Engine!')");
	// IPythonScriptPlugin::Get()->ExecPythonCommand(*PythonCode);
}

void UBuildLevelWorldSubsystem::OnLevelPrepared()
{
	auto EventSystem = GetWorld()->GetSubsystem<UProjectJEventSystem>();
	EventSystem->OnCreateLevel.AddUObject(this, &UBuildLevelWorldSubsystem::CreateNewLevel);
	EventSystem->OnCreateCharacter.AddUObject(this, &UBuildLevelWorldSubsystem::CreateNewCharacter);
	EventSystem->OnCreateLandmark.AddUObject(this, &UBuildLevelWorldSubsystem::CreateNewLandmark);
	EventSystem->OnSaveLevel.AddUObject(this, &UBuildLevelWorldSubsystem::SaveLevel);
}

void UBuildLevelWorldSubsystem::CreateNewLevel(const FName& InLevelName)
{
	auto LevelTable = GetDefault<UProjectJDataTableSettings>()->LevelTable.LoadSynchronous();

	// 检查是否有重复的Row
	auto RowNames = LevelTable->GetRowNames();
	if (RowNames.Contains(InLevelName))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Level %s already exists!"), *InLevelName.ToString()));
		return;
	}

	FString Pinyin = UPythonBridge::Get()->ToPinyin(InLevelName.ToString());
	CreateLuaScript(InLevelName, Pinyin, EProjectJLuaInstanceType::Level);
	
	FProjectJLevelConfig NewLevelConfig;
	NewLevelConfig.LevelName = FText::FromName(InLevelName);
	NewLevelConfig.LuaScriptName = FName(*Pinyin);
	LevelTable->AddRow(InLevelName, NewLevelConfig);
	UEditorLoadingAndSavingUtils::SavePackages({ LevelTable->GetPackage() }, false);
	FDataTableEditorUtils::BroadcastPostChange(const_cast<UDataTable*>(LevelTable), FDataTableEditorUtils::EDataTableChangeInfo::RowList);

}

void UBuildLevelWorldSubsystem::CreateNewCharacter(const FName& InCharacterName)
{
	auto CharacterTable = GetDefault<UProjectJDataTableSettings>()->CharacterTable.LoadSynchronous();
	// 检查是否有重复的Row
	auto RowNames = CharacterTable->GetRowNames();
	if (RowNames.Contains(InCharacterName))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Character %s already exists!"), *InCharacterName.ToString()));
		return;
	}
	FString Pinyin = UPythonBridge::Get()->ToPinyin(InCharacterName.ToString());
	CreateLuaScript(InCharacterName, Pinyin, EProjectJLuaInstanceType::Character);
	
	FProjectJCharacterConfig NewCharacterConfig;
	NewCharacterConfig.Name = FText::FromName(InCharacterName);
	NewCharacterConfig.LuaScriptName = FName(*Pinyin);
	CharacterTable->AddRow(InCharacterName, NewCharacterConfig);
	UEditorLoadingAndSavingUtils::SavePackages({ CharacterTable->GetPackage() }, false);
	FDataTableEditorUtils::BroadcastPostChange(const_cast<UDataTable*>(CharacterTable), FDataTableEditorUtils::EDataTableChangeInfo::RowList);
}

void UBuildLevelWorldSubsystem::CreateNewLandmark(const FName& InLandmarkName)
{
	auto LandmarkTable = GetDefault<UProjectJDataTableSettings>()->LandmarkTable.LoadSynchronous();
	// 检查是否有重复的Row
	auto RowNames = LandmarkTable->GetRowNames();
	if (RowNames.Contains(InLandmarkName))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Landmark %s already exists!"), *InLandmarkName.ToString()));
		return;
	}
	FString Pinyin = UPythonBridge::Get()->ToPinyin(InLandmarkName.ToString());
	CreateLuaScript(InLandmarkName, Pinyin, EProjectJLuaInstanceType::Landmark);
	
	FProjectJLandmarkConfig NewLandmarkConfig;
	NewLandmarkConfig.Name = FText::FromName(InLandmarkName);
	NewLandmarkConfig.LuaScriptName = FName(*Pinyin);
	LandmarkTable->AddRow(InLandmarkName, NewLandmarkConfig);
	UEditorLoadingAndSavingUtils::SavePackages({ LandmarkTable->GetPackage() }, false);
	FDataTableEditorUtils::BroadcastPostChange(const_cast<UDataTable*>(LandmarkTable), FDataTableEditorUtils::EDataTableChangeInfo::RowList);
}

void UBuildLevelWorldSubsystem::SaveLevel(const FName& Name, const FProjectJLevelConfig& ProjectJLevelConfig)
{
	auto LevelTable = GetDefault<UProjectJDataTableSettings>()->LevelTable.LoadSynchronous();
	auto RowNames = LevelTable->GetRowNames();
	if (!RowNames.Contains(Name))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Level %s not exists!"), *Name.ToString()));
		return;
	}

	// 获取对应的Row， 更新数据
	auto LevelRow = LevelTable->FindRow<FProjectJLevelConfig>(Name, TEXT("SaveLevel"));
	check(LevelRow);
	LevelRow->Characters = ProjectJLevelConfig.Characters;
	LevelRow->Landmarks = ProjectJLevelConfig.Landmarks;
	LevelRow->NavPoints = ProjectJLevelConfig.NavPoints;
	UEditorLoadingAndSavingUtils::SavePackages({ LevelTable->GetPackage() }, false);
	FDataTableEditorUtils::BroadcastPostChange(const_cast<UDataTable*>(LevelTable), FDataTableEditorUtils::EDataTableChangeInfo::RowData);
}

void UBuildLevelWorldSubsystem::CreateLuaScript(const FName& InRowName, const FString& InLuaScriptName, EProjectJLuaInstanceType InType)
{
	FString GLuaSrcRelativePath;
	const char* TemplateFilePath;
	switch (InType) {
		case EProjectJLuaInstanceType::Level:
			GLuaSrcRelativePath = TEXT("Script/Levels/");
			TemplateFilePath = "Config/LuaTemplates/LevelTemplate.lua";
			break;
		case EProjectJLuaInstanceType::Character:
			GLuaSrcRelativePath = TEXT("Script/Characters/");
			TemplateFilePath = "Config/LuaTemplates/CharacterTemplate.lua";
			break;
		case EProjectJLuaInstanceType::Landmark:
			GLuaSrcRelativePath = TEXT("Script/Landmarks/");
			TemplateFilePath = "Config/LuaTemplates/LandmarkTemplate.lua";
			break;
		default:
			UE_LOG(LogTemp, Error, TEXT("Unknown LuaFileType %d"), (int32)InType);
			return;
	}

	FString GLuaSrcFullPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() + GLuaSrcRelativePath);
	auto FileName = FString::Printf(TEXT("%s%s.lua"), *GLuaSrcFullPath, *InLuaScriptName);
	int32 SubIndex = 0;
	while (FPaths::FileExists(FileName))
	{
		FileName = FString::Printf(TEXT("%s%s_%d.lua"), *GLuaSrcFullPath, *InLuaScriptName, SubIndex);
	}

	auto TemplateFileFullPath = FPaths::ProjectConfigDir() / TemplateFilePath;
	FString Content;
	FFileHelper::LoadFileToString(Content, *TemplateFileFullPath);

	// 时间格式 YYYY-MM-DD
	Content = Content.Replace(TEXT("{DateTime}"), *FDateTime::Now().ToString(TEXT("%Y-%m-%d"))).Replace(TEXT("{RowName}"), *InRowName.ToString());
	FFileHelper::SaveStringToFile(Content, *FileName, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
}
