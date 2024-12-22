// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectJEditorBFL.h"

bool UProjectJEditorBFL::CreateLuaScript(const FName& InRowName, const FString& InLuaScriptName, EProjectJLuaInstanceType InType, bool ReportErrorDialog)
{
	FString GLuaSrcRelativePath;
	const char* TemplateFilePath;
	bool AllowNewName = true;
	switch (InType)
	{
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
		case EProjectJLuaInstanceType::Ability:
			GLuaSrcRelativePath = TEXT("Script/Abilities/");
			TemplateFilePath = "Config/LuaTemplates/AbilityTemplate.lua";
			AllowNewName = false;
			break;
		case EProjectJLuaInstanceType::Prop:
			GLuaSrcRelativePath = TEXT("Script/Props/");
			TemplateFilePath = "Config/LuaTemplates/PropTemplate.lua";
			AllowNewName = false;
			break;
		default:
			UE_LOG(LogTemp, Error, TEXT("Unknown LuaFileType %d"), (int32)InType);
			return false;
	}

	FString GLuaSrcFullPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() + GLuaSrcRelativePath);
	auto FileName = FString::Printf(TEXT("%s%s.lua"), *GLuaSrcFullPath, *InLuaScriptName);
	int32 SubIndex = 0;
	if (!AllowNewName)
	{
		if (FPaths::FileExists(FileName))
		{
			// 显示Dialog
			// 弹出提示框
			if (ReportErrorDialog)
			{
				FText DialogText = FText::Format(
								NSLOCTEXT("AutoDWEditor", "FileAlreadyExists", "Lua file ({0}) is already existed!"),
								FText::FromString(FileName)
						   );
				FMessageDialog::Open(EAppMsgType::Ok, DialogText);
			}
			return false;
		}
	}
	
	while (FPaths::FileExists(FileName))
	{
		FileName = FString::Printf(TEXT("%s%s_%d.lua"), *GLuaSrcFullPath, *InLuaScriptName, SubIndex);
	}

	auto TemplateFileFullPath = FPaths::ProjectConfigDir() / TemplateFilePath;
	FString Content;
	FFileHelper::LoadFileToString(Content, *TemplateFileFullPath);

	// 时间格式 YYYY-MM-DD
	Content = Content.Replace(TEXT("{DateTime}"), *FDateTime::Now().ToString(TEXT("%Y-%m-%d"))).Replace(
		TEXT("{RowName}"), *InRowName.ToString()).Replace(TEXT("{LuaScriptName}"), *InLuaScriptName);
	FFileHelper::SaveStringToFile(Content, *FileName, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
	return true;
}
