// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildLevelWorldSubsystem.h"

#include "DataTableEditorUtils.h"
#include "FileHelpers.h"
#include "ProjectJEditorBFL.h"
#include "PythonBridge.h"
#include "BuildLevel/ProjectJBuildLevelGameMode.h"
#include "Game/ProjectJGameMode.h"
#include "Core/DeveloperSettings/ProjectJDataTableSettings.h"
#include "Core/System/ProjectJEventSystem.h"
#include "Types/ProjectJCharacterConfig.h"
#include "Types/ProjectJLandmarkConfig.h"
#include "Types/ProjectJUtilityConfig.h"


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
	EventSystem->OnCreateCharacter.AddUObject(this, &UBuildLevelWorldSubsystem::CreateNewCharacter);
	EventSystem->OnCreateLandmark.AddUObject(this, &UBuildLevelWorldSubsystem::CreateNewLandmark);
	EventSystem->OnCreateUtility.AddUObject(this, &UBuildLevelWorldSubsystem::CreateNewUtility);
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
	UProjectJEditorBFL::CreateLuaScript(InCharacterName, Pinyin, EProjectJLuaInstanceType::Character);
	
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
	UProjectJEditorBFL::CreateLuaScript(InLandmarkName, Pinyin, EProjectJLuaInstanceType::Landmark);
	
	FProjectJLandmarkConfig NewLandmarkConfig;
	NewLandmarkConfig.Name = FText::FromName(InLandmarkName);
	NewLandmarkConfig.LuaScriptName = FName(*Pinyin);
	LandmarkTable->AddRow(InLandmarkName, NewLandmarkConfig);
	UEditorLoadingAndSavingUtils::SavePackages({ LandmarkTable->GetPackage() }, false);
	FDataTableEditorUtils::BroadcastPostChange(const_cast<UDataTable*>(LandmarkTable), FDataTableEditorUtils::EDataTableChangeInfo::RowList);
}

void UBuildLevelWorldSubsystem::CreateNewUtility(const FName& InUtilityName)
{
	auto UtilityTable = GetDefault<UProjectJDataTableSettings>()->UtilityTable.LoadSynchronous();
	// 检查是否有重复的Row
	auto RowNames = UtilityTable->GetRowNames();
	if (RowNames.Contains(InUtilityName))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Utility %s already exists!"), *InUtilityName.ToString()));
		return;
	}
	FString Pinyin = UPythonBridge::Get()->ToPinyin(InUtilityName.ToString());
	UProjectJEditorBFL::CreateLuaScript(InUtilityName, Pinyin, EProjectJLuaInstanceType::Utility);
	
	FProjectJUtilityConfig NewUtilityConfig;
	NewUtilityConfig.Name = FText::FromName(InUtilityName);
	NewUtilityConfig.LuaScriptName = FName(*Pinyin);
	UtilityTable->AddRow(InUtilityName, NewUtilityConfig);
	UEditorLoadingAndSavingUtils::SavePackages({ UtilityTable->GetPackage() }, false);
	FDataTableEditorUtils::BroadcastPostChange(const_cast<UDataTable*>(UtilityTable), FDataTableEditorUtils::EDataTableChangeInfo::RowList);
}