#include "ProjectJEditor.h"

#include "BuildLevelWorldSubsystem.h"
#include "DataTableEditorUtils.h"
#include "FileHelpers.h"
#include "ProjectJGameEditorStyle.h"
#include "ProjectJLuaFunction.h"
#include "GameplayAbilitiesEditorModule.h"
#include "ProjectJEditorBFL.h"
#include "PythonBridge.h"
#include "Editor/UnrealEdEngine.h"
#include "Engine/GameInstance.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "ToolMenu.h"
#include "ToolMenus.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Core/DeveloperSettings/ProjectJDataTableSettings.h"
#include "Core/DeveloperSettings/ProjectJGeneralSettings.h"
#include "Core/DeveloperSettings/ProjectJPropertyHelper.h"
#include "Game/ProjectJEffectActor.h"
#include "Types/ProjectJDungeonConfig.h"
#include "Types/Item/ProjectJPropConfig.h"
#include "UObject/UObjectIterator.h"
#include "UI/SpecialUI/ProjectJBigMap.h"

#define LOCTEXT_NAMESPACE "FProjectJEditorModule"


static bool HasPlayWorld()
{
	return GEditor->PlayWorld != nullptr;
}

static bool HasNoPlayWorld()
{
	return !HasPlayWorld();
}

static void OpenCommonMap_Clicked(const FString MapPath)
{
	if (ensure(MapPath.Len()))
	{
		GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(MapPath);
	}
}

static void TestAction(UToolMenu* InMenu)
{
	UE_LOG(LogTemp, Warning, TEXT("TestAction"));

	const FToolMenuContext Context = InMenu->Context;
	if (Context.IsEditing())
	{
		UE_LOG(LogTemp, Warning, TEXT("Context.IsValid()"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Context.IsNotValid()"));
	}
}

static TSharedRef<SWidget> GetCommonMapsDropdown()
{
	FMenuBuilder MenuBuilder(true, nullptr);
	
	// for (const FSoftObjectPath& Path : GetDefault<UProjectJDeveloperSettings>()->CommonEditorMaps)
	// {
	// 	if (!Path.IsValid())
	// 	{
	// 		continue;
	// 	}
	// 	
	// 	const FText DisplayName = FText::FromString(Path.GetAssetName());
	// 	MenuBuilder.AddMenuEntry(
	// 		DisplayName,
	// 		LOCTEXT("CommonPathDescription", "Opens this map in the editor"),
	// 		FSlateIcon(),
	// 		FUIAction(
	// 			FExecuteAction::CreateStatic(&OpenCommonMap_Clicked, Path.ToString()),
	// 			FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
	// 			FIsActionChecked(),
	// 			FIsActionButtonVisible::CreateStatic(&HasNoPlayWorld)
	// 		)
	// 	);
	// }

	return MenuBuilder.MakeWidget();
}

static TSharedRef<SWidget> ProjectJMenu()
{
	FMenuBuilder MenuBuilder(true, nullptr);
	
	// for (const FSoftObjectPath& Path : GetDefault<UProjectJDeveloperSettings>()->CommonEditorMaps)
	// {
	// 	if (!Path.IsValid())
	// 	{
	// 		continue;
	// 	}
	// 	
	// 	const FText DisplayName = FText::FromString(Path.GetAssetName());
	// 	MenuBuilder.AddMenuEntry(
	// 		DisplayName,
	// 		LOCTEXT("CommonPathDescription", "Opens this map in the editor"),
	// 		FSlateIcon(),
	// 		FUIAction(
	// 			FExecuteAction::CreateStatic(&OpenCommonMap_Clicked, Path.ToString()),
	// 			FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
	// 			FIsActionChecked(),
	// 			FIsActionButtonVisible::CreateStatic(&HasNoPlayWorld)
	// 		)
	// 	);
	// }

	const FText DisplayName = FText::FromString(TEXT("Test"));
	MenuBuilder.AddMenuEntry(
		DisplayName,
		LOCTEXT("CommonPathDescription", "Opens this map in the editor"),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateStatic(&OpenCommonMap_Clicked, FString(TEXT("Test"))),
			FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateStatic(&HasNoPlayWorld)
		)
	);

	return MenuBuilder.MakeWidget();
}

static void TestAction()
{
	UE_LOG(LogTemp, Warning, TEXT("TestAction"));
}

static void IntervalMigrateLuaTemplate(EProjectJLuaInstanceType InType)
{
	const char* TemplateFilePath;
	FString GAbilityLuaSrcRelativePath;
	switch (InType) {
		case EProjectJLuaInstanceType::Dungeon:
			TemplateFilePath = "Config/LuaTemplates/DungeonTemplate.lua";
			GAbilityLuaSrcRelativePath = TEXT("Script/Dungeons/");
			break;
		case EProjectJLuaInstanceType::Character:
			TemplateFilePath = "Config/LuaTemplates/CharacterTemplate.lua";
			GAbilityLuaSrcRelativePath = TEXT("Script/Characters/");
			break;
		case EProjectJLuaInstanceType::Landmark:
			TemplateFilePath = "Config/LuaTemplates/LandmarkTemplate.lua";
			GAbilityLuaSrcRelativePath = TEXT("Script/Landmarks/");
			break;
		case EProjectJLuaInstanceType::Ability:
			TemplateFilePath = "Config/LuaTemplates/AbilityTemplate.lua";
			GAbilityLuaSrcRelativePath = TEXT("Script/Abilities/");
			break;
		case EProjectJLuaInstanceType::Prop:
			TemplateFilePath = "Config/LuaTemplates/PropTemplate.lua";
			GAbilityLuaSrcRelativePath = TEXT("Script/Props/");
			break;
		case EProjectJLuaInstanceType::Utility:
			TemplateFilePath = "Config/LuaTemplates/UtilityTemplate.lua";
			GAbilityLuaSrcRelativePath = TEXT("Script/Utilities/");
			break;
		case EProjectJLuaInstanceType::Spell:
			TemplateFilePath = "Config/LuaTemplates/SpellTemplate.lua";
			GAbilityLuaSrcRelativePath = TEXT("Script/Spells/");
			break;
		default:
			return;
	}
	
	auto FullFilePath = FPaths::ProjectConfigDir() / TemplateFilePath;
	FString LuaTemplateContent;
	FFileHelper::LoadFileToString(LuaTemplateContent, *FullFilePath);

	// 将每一个function提取为FProjectJLuaFunction结构， 每一个函数的Comment保存在函数名上方---[[[与---]]]之间
	TArray<FProjectJLuaFunction> TemplateFunctions;
	FRegexPattern FunctionPattern(TEXT("function\\s+M:(\\w+)\\(([^)]*)\\)"));
	FRegexPattern CommentPattern(TEXT("(?s)---\\[\\[\\[(.*?)---\\]\\]\\]"));
	FRegexMatcher FunctionMatcher(FunctionPattern, LuaTemplateContent);
	FRegexMatcher CommentMatcher(CommentPattern, LuaTemplateContent);
	
	while (FunctionMatcher.FindNext())
	{
		FProjectJLuaFunction LuaFunction;
		LuaFunction.FunctionName = FunctionMatcher.GetCaptureGroup(1);
		auto Parameters = FunctionMatcher.GetCaptureGroup(2);
		// 去除空格
		Parameters = Parameters.Replace(TEXT(" "), TEXT(""));
		Parameters.ParseIntoArray(LuaFunction.Parameters, TEXT(","));
		
		if (CommentMatcher.FindNext())
		{
			LuaFunction.Comment = CommentMatcher.GetCaptureGroup(1);
		}

		// 写入函数的默认内容, 在function xxx() end之间
		FRegexPattern FunctionContentPattern(
			FString::Printf(TEXT("(?s)function\\s+M:%s\\(([^)]*)\\)(.*?)\\bend\\b"), *LuaFunction.FunctionName));
		FRegexMatcher FunctionContentMatcher(FunctionContentPattern, LuaTemplateContent);
	
		if (FunctionContentMatcher.FindNext())
		{
			LuaFunction.DefaultContent = FunctionContentMatcher.GetCaptureGroup(2);
		}

		TemplateFunctions.Add(LuaFunction);
	}

	

	FString GAbilityLuaSrcFullPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() + GAbilityLuaSrcRelativePath);
	
	TArray<FString> LuaFiles;
	IFileManager::Get().FindFilesRecursive(LuaFiles, *GAbilityLuaSrcFullPath, TEXT("*.lua"), true, false);

	for (const FString& LuaFile : LuaFiles)
	{
		FString LuaContent;
		FFileHelper::LoadFileToString(LuaContent, *LuaFile);

		// Extract function signatures and comments from the Lua script
		TArray<FProjectJLuaFunction> LuaFunctions;
		TArray<EProjectJCompareResult> CompareResults;
		FRegexMatcher LuaFunctionMatcher(FunctionPattern, LuaContent);
		FRegexMatcher LuaCommentMatcher(CommentPattern, LuaContent);

		while (LuaFunctionMatcher.FindNext())
		{
			FProjectJLuaFunction LuaFunction;
			LuaFunction.FunctionName = LuaFunctionMatcher.GetCaptureGroup(1);
			auto Parameters = LuaFunctionMatcher.GetCaptureGroup(2);
			// 去除空格
			Parameters = Parameters.Replace(TEXT(" "), TEXT(""));
			Parameters.ParseIntoArray(LuaFunction.Parameters, TEXT(","));

			if (LuaCommentMatcher.FindNext())
			{
				LuaFunction.Comment = LuaCommentMatcher.GetCaptureGroup(1);
			}

			LuaFunctions.Add(LuaFunction);
		}
		
		// Compare the functions and comments
		// 更新当前的lua文件
		for (const FProjectJLuaFunction& TemplateFunction : TemplateFunctions)
		{
			bool bFunctionFound = false;
			for (const FProjectJLuaFunction& LuaFunction : LuaFunctions)
			{
				if (TemplateFunction.FunctionName == LuaFunction.FunctionName)
				{
					bFunctionFound = true;
					if (TemplateFunction.Parameters != LuaFunction.Parameters || TemplateFunction.Comment != LuaFunction.Comment)
					{
						// 打印Parameters 和 Comment
						UE_LOG(LogTemp, Warning, TEXT("-------LuaFile: %s NotMatch-------"), *LuaFile);
						UE_LOG(LogTemp, Warning, TEXT("TemplateFunction: %s, Parameters: %s, Comment: %s"), *TemplateFunction.FunctionName, *FString::Join(TemplateFunction.Parameters, TEXT(", ")), *TemplateFunction.Comment);
						UE_LOG(LogTemp, Warning, TEXT("LuaFunction: %s, Parameters: %s, Comment: %s"), *LuaFunction.FunctionName, *FString::Join(LuaFunction.Parameters, TEXT(", ")), *LuaFunction.Comment);
						CompareResults.Add(EProjectJCompareResult::NotMatch);
					}
					else
					{
						CompareResults.Add(EProjectJCompareResult::Match);
					}
					break;
				}
			}
			
			if (!bFunctionFound)
			{
				CompareResults.Add(EProjectJCompareResult::Missing);
			}
		}

		// 根据CompareResults， 更新Lua文件， 如果结果是NotMatch， 更新Parameters和Comment， 如果是Missing， 添加新的Function
		bool NeedSave = false;
		for (int32 i = 0; i < CompareResults.Num(); ++i)
		{
			const FProjectJLuaFunction& TemplateFunction = TemplateFunctions[i];
			switch (CompareResults[i])
			{
				case EProjectJCompareResult::NotMatch:
					{
						NeedSave = true;
						// Update Parameters and Comment
						FRegexPattern UpdatePattern(
							FString::Printf(TEXT("function\\s+M:%s\\(([^)]*)\\)"), *TemplateFunction.FunctionName));
						FRegexMatcher UpdateMatcher(UpdatePattern, LuaContent);
						if (UpdateMatcher.FindNext())
						{
							FString NewFunction = FString::Printf(
								TEXT("function M:%s(%s)"), *TemplateFunction.FunctionName,
								*FString::Join(TemplateFunction.Parameters, TEXT(", ")));
							LuaContent = LuaContent.Replace(*UpdateMatcher.GetCaptureGroup(0), *NewFunction);

							int32 LuaFuncIndex = -1;
							for (int32 m = 0; m < LuaFunctions.Num(); ++m)
							{
								if (LuaFunctions[m].FunctionName == TemplateFunction.FunctionName)
								{
									LuaFuncIndex = m;
									break;
								}
							}

							if (LuaFuncIndex != -1)
							{
								const FProjectJLuaFunction& LuaFunction = LuaFunctions[LuaFuncIndex];
								LuaContent = LuaContent.Replace(*LuaFunction.Comment, *TemplateFunction.Comment);
							}
							else
							{
								UE_LOG(LogTemp, Error, TEXT("LuaFunction Not Found: %s"), *TemplateFunction.FunctionName);
							}
						}
						break;
					}
				case EProjectJCompareResult::Missing:
					{
						NeedSave = true;
						// Add new Function
						FString NewFunction = FString::Printf(
							TEXT("\n---[[[%s---]]]\nfunction M:%s(%s)%send\n"), *TemplateFunction.Comment,
							*TemplateFunction.FunctionName, *FString::Join(TemplateFunction.Parameters, TEXT(", ")),*TemplateFunction.DefaultContent);

						// 在LuaContent的return M之前插入
						LuaContent = LuaContent.Replace(TEXT("return M"), *NewFunction);
						LuaContent.Append(TEXT("\nreturn M"));
						break;
					}
				default:
					break;
			}
		}

		// 检查local M = {} 中 {} 里的内容是否相同
		FRegexPattern LocalPattern(TEXT("local\\s+M\\s*=\\s*\\{([^}]*)\\}"));
		FRegexMatcher LocalMatcher(LocalPattern, LuaContent); // for Lua Content
		// Template
		FRegexMatcher TemplateLocalMatcher(LocalPattern, LuaTemplateContent);
		
		if (LocalMatcher.FindNext() && TemplateLocalMatcher.FindNext())
		{
			FString LocalDef = LocalMatcher.GetCaptureGroup(1);
			FString TemplateDef = TemplateLocalMatcher.GetCaptureGroup(1);
			if (LocalDef != TemplateDef)
			{
				NeedSave = true;
				LuaContent = LuaContent.Replace(*LocalDef, *TemplateDef);
			}
		}
		
		if (NeedSave)
		{
			FFileHelper::SaveStringToFile(LuaContent, *LuaFile, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
		}
	}
}

static TArray<FString> MakeTableRowValues(UDataTable* InTable, FString InPrefix)
{
	TArray<FString> Ret;
	auto RowNames = InTable->GetRowNames();
	auto PythonBridge = UPythonBridge::Get();
	TArray<FString> PinyinRowNames;
	for (int32 i = 0; i < RowNames.Num(); i++)
	{
		auto Pinyin = PythonBridge->ToPinyin(RowNames[i].ToString());
		int SubIndex = 0;
		while (PinyinRowNames.Contains(Pinyin))
		{
			Pinyin = FString::Printf(TEXT("%s_%d"), *Pinyin, SubIndex);
		}
		PinyinRowNames.Add(Pinyin);
	}

	for (int32 i = 0; i < RowNames.Num(); i++)
	{
		auto Pinyin = PinyinRowNames[i];
		Ret.Add(
			FString::Printf(TEXT("_G.%s_%s = \"%s\""), *InPrefix, *Pinyin, *RowNames[i].ToString()));
	}

	return Ret;
}

static void CreateStaticVariableLua()
{
	static FString GLuaSrcRelativePath = TEXT("Script/");
	static FString GLuaSrcFullPath = FPaths::ConvertRelativePathToFull(
		FPaths::ProjectContentDir() + GLuaSrcRelativePath);
	const auto GlobalFileName = FString::Printf(TEXT("%sProjectJGlobal.lua"), *GLuaSrcFullPath);
	const auto GameplayTagsFileName = FString::Printf(TEXT("%sProjectJGameplayTags.lua"), *GLuaSrcFullPath);

	// 如果存在，先删除
	if (FPaths::FileExists(GlobalFileName))
	{
		FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*GlobalFileName);
	}

	if (FPaths::FileExists(GameplayTagsFileName))
	{
		FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*GameplayTagsFileName);
	}

	auto RelativeFilePath = "Config/LuaTemplates/StaticVariableTemplate.lua";
	auto FullFilePath = FPaths::ProjectConfigDir() / RelativeFilePath;
	check(FPaths::FileExists(FullFilePath))
	FString GlobalContent;
	FFileHelper::LoadFileToString(GlobalContent, *FullFilePath);
	GlobalContent = GlobalContent
		// 时间格式 YYYY-MM-DD
		.Replace(TEXT("{DateTime}"), *FDateTime::Now().ToString(TEXT("%Y-%m-%d")));

	// 读取ProjectJGlobal
	auto ProjectJGlobalCPPPath = FPaths::ConvertRelativePathToFull(
		FPaths::ProjectDir() + TEXT("Source/ProjectJ/ProjectJGlobal.cpp"));
	FString ProjectJGlobalContent;
	FFileHelper::LoadFileToString(ProjectJGlobalContent, *ProjectJGlobalCPPPath);
	//取ProjectJGlobal.cpp为字符串，然后通过正则匹配，获取变量名和值
	// Todo: 暂时并未导出TMap和TArray
	TArray<FString> ExternVariables;
	FRegexPattern ExternPattern(TEXT("const\\s+FName\\s+(\\w+)\\s*=\\s*TEXT\\(\"([^\"]+)\"\\);"));
	FRegexMatcher Matcher(ExternPattern, ProjectJGlobalContent);

	while (Matcher.FindNext())
	{
		FString VarName = Matcher.GetCaptureGroup(1);
		FString VarValue = Matcher.GetCaptureGroup(2);
		ExternVariables.Add(FString::Printf(TEXT("_G.%s = \"%s\""), *VarName, *VarValue));
	}

	// 读取项目中全部的Montage文件
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	// Define a filter to search for UAnimMontage assets
	FARFilter Filter;
	Filter.ClassPaths.Add(UAnimMontage::StaticClass()->GetClassPathName());
	Filter.bRecursiveClasses = true;

	// Get the assets
	TArray<FAssetData> AssetData;
	AssetRegistry.GetAssets(Filter, AssetData);
	for (const FAssetData& Data : AssetData)
	{
		// 获取TSoftObjectPtr<UAnimMontage>的路径
		FString SoftPath = Data.ToSoftObjectPath().ToString();
		ExternVariables.Add(FString::Printf(TEXT("_G.Montage_%s = \"%s\""), *Data.AssetName.ToString(), *SoftPath));
	}
	
	auto PropertyHelper = GetDefault<UProjectJPropertyHelper>();
	for (const auto& Pair : PropertyHelper->EffectActorClassMap)
	{
		ExternVariables.Add(FString::Printf(TEXT("_G.%s = \"%s\""), *Pair.Key, *Pair.Key));
	}
	
	// Append the extracted variables to the Lua content
	FString Content = FString::Join(ExternVariables, TEXT("\n"));

	GlobalContent = GlobalContent.Replace(TEXT("---{Content}"), *Content);

	FFileHelper::SaveStringToFile(GlobalContent, *GlobalFileName, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);

	//////// Write GameplayTas
	FString GameplayTagsContent;
	FFileHelper::LoadFileToString(GameplayTagsContent, *FullFilePath);
	GameplayTagsContent = GameplayTagsContent
		// 时间格式 YYYY-MM-DD
		.Replace(TEXT("{DateTime}"), *FDateTime::Now().ToString(TEXT("%Y-%m-%d")));

	// 读取ProjectJGameplayTags.cpp
	auto ProjectJGameplayTagsCPPPath = FPaths::ConvertRelativePathToFull(
		FPaths::ProjectDir() + TEXT("Source/ProjectJ/ProjectJGameplayTags.cpp"));
	FString ProjectJGameplayTagsContent;
	FFileHelper::LoadFileToString(ProjectJGameplayTagsContent, *ProjectJGameplayTagsCPPPath);
	//取ProjectJGlobal.cpp为字符串，然后通过正则匹配，获取变量名和值
	// Todo: 暂时并未导出TMap和TArray
	TArray<FString> GameplayTagsExternVariables;
	FRegexPattern GameplayTagPattern(
		TEXT("UE_DEFINE_GAMEPLAY_TAG_COMMENT\\((\\w+),\\s*\"([^\"]+)\",\\s*\"([^\"]+)\"\\);"));
	FRegexMatcher GameplayTagMatcher(GameplayTagPattern, ProjectJGameplayTagsContent);

	while (GameplayTagMatcher.FindNext())
	{
		FString TagName = GameplayTagMatcher.GetCaptureGroup(1);
		FString TagValue = GameplayTagMatcher.GetCaptureGroup(2);
		GameplayTagsExternVariables.Add(FString::Printf(TEXT("_G.%s = \"%s\""), *TagName, *TagValue));
	}

	Content = FString::Join(GameplayTagsExternVariables, TEXT("\n"));
	GameplayTagsContent = GameplayTagsContent.Replace(TEXT("---{Content}"), *Content);
	FFileHelper::SaveStringToFile(GameplayTagsContent, *GameplayTagsFileName,
	                              FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
	
	///// Write CharacterTable
	FString TableContent;
	FFileHelper::LoadFileToString(TableContent, *FullFilePath);
	TableContent = TableContent
		// 时间格式 YYYY-MM-DD
		.Replace(TEXT("{DateTime}"), *FDateTime::Now().ToString(TEXT("%Y-%m-%d")));


	auto DTSettings = GetDefault<UProjectJDataTableSettings>();
	auto DungeonTableVariables = MakeTableRowValues(DTSettings->DungeonTable.LoadSynchronous(), TEXT("Level"));
	auto CharacterTableVariables = MakeTableRowValues(DTSettings->CharacterTable.LoadSynchronous(), TEXT("Character"));
	auto LandmarkTableVariables = MakeTableRowValues(DTSettings->LandmarkTable.LoadSynchronous(), TEXT("Landmark"));
	auto UtilityTableVariables = MakeTableRowValues(DTSettings->UtilityTable.LoadSynchronous(), TEXT("Utility"));
	auto WeaponTableVariables = MakeTableRowValues(DTSettings->WeaponTable.LoadSynchronous(), TEXT("Weapon"));
	auto ArmorTableVariables = MakeTableRowValues(DTSettings->ArmorTable.LoadSynchronous(), TEXT("Armor"));
	auto PropTableVariables = MakeTableRowValues(DTSettings->PropTable.LoadSynchronous(), TEXT("Prop"));

	// LevelTableVariables\CharacterTableVariables\LandmarkTableVariables 三者都需要写入Content
	Content = FString::Join(DungeonTableVariables, TEXT("\n"));
	Content = Content + TEXT("\n") + FString::Join(CharacterTableVariables, TEXT("\n"));
	Content = Content + TEXT("\n") + FString::Join(LandmarkTableVariables, TEXT("\n"));
	Content = Content + TEXT("\n") + FString::Join(UtilityTableVariables, TEXT("\n"));
	Content = Content + TEXT("\n") + FString::Join(WeaponTableVariables, TEXT("\n"));
	Content = Content + TEXT("\n") + FString::Join(ArmorTableVariables, TEXT("\n"));
	Content = Content + TEXT("\n") + FString::Join(PropTableVariables, TEXT("\n"));
	
	TableContent = TableContent.Replace(TEXT("---{Content}"), *Content);
	auto TablePath = FString::Printf(TEXT("%sTableRows.lua"), *GLuaSrcFullPath);
	FFileHelper::SaveStringToFile(TableContent, *TablePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
}

static void MigrateAbilityTemplate()
{
	IntervalMigrateLuaTemplate(EProjectJLuaInstanceType::Dungeon);
	IntervalMigrateLuaTemplate(EProjectJLuaInstanceType::Character);
	IntervalMigrateLuaTemplate(EProjectJLuaInstanceType::Landmark);
	IntervalMigrateLuaTemplate(EProjectJLuaInstanceType::Utility);
	IntervalMigrateLuaTemplate(EProjectJLuaInstanceType::Ability);
	IntervalMigrateLuaTemplate(EProjectJLuaInstanceType::Prop);
	IntervalMigrateLuaTemplate(EProjectJLuaInstanceType::Spell);
}

static void CreateSpellLuaScript()
{
	// 显示一个对话框，可以输入技能名字， 并且包含确定和取消按钮
	TSharedRef<SWindow> Dialog = SNew(SWindow)
        .Title(FText::FromString("Create Ability Lua Script"))
        .ClientSize(FVector2D(400, 200))
        .SupportsMinimize(false)
        .SupportsMaximize(false);

    TSharedPtr<SEditableTextBox> AbilityNameTextBox;

    Dialog->SetContent(
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .Padding(10)
        .AutoHeight()
        [
            SNew(STextBlock)
            .Text(FText::FromString("Enter Ability Name:"))
        ]
        + SVerticalBox::Slot()
        .Padding(10)
        .AutoHeight()
        [
            SAssignNew(AbilityNameTextBox, SEditableTextBox)
        ]
        + SVerticalBox::Slot()
        .Padding(10)
        .AutoHeight()
        .HAlign(HAlign_Right)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(5)
            [
                SNew(SButton)
                .Text(FText::FromString("OK"))
                .OnClicked_Lambda([Dialog, AbilityNameTextBox]()
                {
                    FString SpellTagName = AbilityNameTextBox->GetText().ToString();
                    if (!SpellTagName.IsEmpty())
                    {
                    	// 转化为拼音
                    	auto PythonBridge = UPythonBridge::Get();
                    	FString Pinyin = PythonBridge->ToPinyin(SpellTagName);
                    	
                        bool CreateSuccess = UProjectJEditorBFL::CreateLuaScript(FName(*SpellTagName), Pinyin, EProjectJLuaInstanceType::Spell);
                    	if (CreateSuccess)
                    	{
                    		Dialog->RequestDestroyWindow();
                    	}
                    }
                    return FReply::Handled();
                })
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(5)
            [
                SNew(SButton)
                .Text(FText::FromString("Cancel"))
                .OnClicked_Lambda([Dialog]()
                {
                    Dialog->RequestDestroyWindow();
                    return FReply::Handled();
                })
            ]
        ]
    );

    FSlateApplication::Get().AddWindow(Dialog);
}

static void CreateAbilityLuaScript()
{
	// 显示一个对话框，可以输入技能名字， 并且包含确定和取消按钮
	TSharedRef<SWindow> Dialog = SNew(SWindow)
        .Title(FText::FromString("Create Ability Lua Script"))
        .ClientSize(FVector2D(400, 200))
        .SupportsMinimize(false)
        .SupportsMaximize(false);

    TSharedPtr<SEditableTextBox> AbilityNameTextBox;

    Dialog->SetContent(
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .Padding(10)
        .AutoHeight()
        [
            SNew(STextBlock)
            .Text(FText::FromString("Enter Ability Name:"))
        ]
        + SVerticalBox::Slot()
        .Padding(10)
        .AutoHeight()
        [
            SAssignNew(AbilityNameTextBox, SEditableTextBox)
        ]
        + SVerticalBox::Slot()
        .Padding(10)
        .AutoHeight()
        .HAlign(HAlign_Right)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(5)
            [
                SNew(SButton)
                .Text(FText::FromString("OK"))
                .OnClicked_Lambda([Dialog, AbilityNameTextBox]()
                {
                    FString AbilityName = AbilityNameTextBox->GetText().ToString();
                    if (!AbilityName.IsEmpty())
                    {
                    	// 转化为拼音
                    	auto PythonBridge = UPythonBridge::Get();
                    	FString Pinyin = PythonBridge->ToPinyin(AbilityName);
                    	
                        bool CreateSuccess = UProjectJEditorBFL::CreateLuaScript(FName(*AbilityName), Pinyin, EProjectJLuaInstanceType::Ability);
                    	if (CreateSuccess)
                    	{
                    		// 更新PropertyHelper 中的静态变量
                    		auto PropertyHelper = GetMutableDefault<UProjectJPropertyHelper>();
                    		PropertyHelper->CHS2AbilityLuaScriptNameMap.Add(FName(*AbilityName), FName(*Pinyin));

                    		// 如何让它只保存在在Config路径下的DefaultPropertyHelperEditor.ini中?
                    		FString DefaultConfigFilePath  = PropertyHelper->GetDefaultConfigFilename();
                    		PropertyHelper->SaveConfig(CPF_Config, *DefaultConfigFilePath);
                    		
                    		Dialog->RequestDestroyWindow();
                    	}
                    }
                    return FReply::Handled();
                })
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(5)
            [
                SNew(SButton)
                .Text(FText::FromString("Cancel"))
                .OnClicked_Lambda([Dialog]()
                {
                    Dialog->RequestDestroyWindow();
                    return FReply::Handled();
                })
            ]
        ]
    );

    FSlateApplication::Get().AddWindow(Dialog);
}

static void CreatePropLuaScript()
{
	auto PythonBridge = UPythonBridge::Get();
	auto PropDT = GetDefault<UProjectJDataTableSettings>()->PropTable.LoadSynchronous();
	auto RowMap = PropDT->GetRowMap();

	// 检查是否存在重复的拼音
	TMap<FString, TArray<FName>> PinyinMap;
	for (const auto& Pair : RowMap)
	{
		auto DesiredPinyin = PythonBridge->ToPinyin(Pair.Key.ToString());
		auto& PinyinArray = PinyinMap.FindOrAdd(DesiredPinyin);
		PinyinArray.Add(Pair.Key);
	}

	for (const auto& Pair : PinyinMap)
	{
		if (Pair.Value.Num() > 1)
		{
			FString ErrorString = FString::Printf(TEXT("重复的拼音: %s, Rows: "), *Pair.Key);
			for (const auto& Name : Pair.Value)
			{
				ErrorString += Name.ToString() + TEXT(", ");
			}
			// 提示框显示ErrorString
			FText DialogText = FText::Format(LOCTEXT("Error", "{0}"), FText::FromString(ErrorString));
			FMessageDialog::Open(EAppMsgType::Ok, DialogText);
			return;
		}
	}

	for (const auto& Pair : RowMap)
	{
		auto PropConfig = reinterpret_cast<FProjectJPropConfig*>(Pair.Value);
		auto DesiredPinyin = PythonBridge->ToPinyin(Pair.Key.ToString());
		UProjectJEditorBFL::CreateLuaScript(Pair.Key, DesiredPinyin, EProjectJLuaInstanceType::Prop, false);
		PropConfig->ExecLuaScriptName = FName(*DesiredPinyin);
	}
	// 保存DataTable
	UEditorLoadingAndSavingUtils::SavePackages({PropDT->GetPackage()}, false);
	FDataTableEditorUtils::BroadcastPostChange(const_cast<UDataTable*>(PropDT),
	                                           FDataTableEditorUtils::EDataTableChangeInfo::RowData);

	// 检查是否存在未被使用的脚本
	FString GLuaSrcFullPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() + "Script/Props/");
	TArray<FString> LuaFiles;
	IFileManager::Get().FindFilesRecursive(LuaFiles, *GLuaSrcFullPath, TEXT("*.lua"), true, false);
	TSet<FString> UsedScripts;
	for (const auto& Pair : RowMap)
	{
		auto PropConfig = reinterpret_cast<FProjectJPropConfig*>(Pair.Value);
		UsedScripts.Add(PropConfig->ExecLuaScriptName.ToString() + TEXT(".lua"));
	}

	TArray<FString> UnusedScripts;
	for (const FString& LuaFile : LuaFiles)
	{
		if (!UsedScripts.Contains(FPaths::GetCleanFilename(LuaFile)))
		{
			UnusedScripts.Add(LuaFile);
		}
	}

	if (UnusedScripts.Num() > 0)
	{
		FString UnusedScriptsList = FString::Join(UnusedScripts, TEXT("\n"));
		FText DialogText = FText::Format(
			LOCTEXT("UnusedScripts", "[Warning] 存在未被使用的脚本:\n{0}"), FText::FromString(UnusedScriptsList));
		FMessageDialog::Open(EAppMsgType::Ok, DialogText);
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("NoUnusedScripts", "更新成功"));
	}
}

static void CreateDungeonLuaScript()
{
	auto PythonBridge = UPythonBridge::Get();
	auto PropDT = GetDefault<UProjectJDataTableSettings>()->DungeonTable.LoadSynchronous();
	auto RowMap = PropDT->GetRowMap();

	// 检查是否存在重复的拼音
	TMap<FString, TArray<FName>> PinyinMap;
	for (const auto& Pair : RowMap)
	{
		auto DesiredPinyin = PythonBridge->ToPinyin(Pair.Key.ToString());
		auto& PinyinArray = PinyinMap.FindOrAdd(DesiredPinyin);
		PinyinArray.Add(Pair.Key);
	}

	for (const auto& Pair : PinyinMap)
	{
		if (Pair.Value.Num() > 1)
		{
			FString ErrorString = FString::Printf(TEXT("重复的拼音: %s, Rows: "), *Pair.Key);
			for (const auto& Name : Pair.Value)
			{
				ErrorString += Name.ToString() + TEXT(", ");
			}
			// 提示框显示ErrorString
			FText DialogText = FText::Format(LOCTEXT("Error", "{0}"), FText::FromString(ErrorString));
			FMessageDialog::Open(EAppMsgType::Ok, DialogText);
			return;
		}
	}

	for (const auto& Pair : RowMap)
	{
		auto PropConfig = reinterpret_cast<FProjectJDungeonConfig*>(Pair.Value);
		auto DesiredPinyin = PythonBridge->ToPinyin(Pair.Key.ToString());
		UProjectJEditorBFL::CreateLuaScript(Pair.Key, DesiredPinyin, EProjectJLuaInstanceType::Dungeon, false);
		PropConfig->LuaScriptName = FName(*DesiredPinyin);
	}
	// 保存DataTable
	UEditorLoadingAndSavingUtils::SavePackages({PropDT->GetPackage()}, false);
	FDataTableEditorUtils::BroadcastPostChange(const_cast<UDataTable*>(PropDT),
	                                           FDataTableEditorUtils::EDataTableChangeInfo::RowData);

	// 检查是否存在未被使用的脚本
	FString GLuaSrcFullPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() + "Script/Dungeons/");
	TArray<FString> LuaFiles;
	IFileManager::Get().FindFilesRecursive(LuaFiles, *GLuaSrcFullPath, TEXT("*.lua"), true, false);
	TSet<FString> UsedScripts;
	for (const auto& Pair : RowMap)
	{
		auto PropConfig = reinterpret_cast<FProjectJDungeonConfig*>(Pair.Value);
		UsedScripts.Add(PropConfig->LuaScriptName.ToString() + TEXT(".lua"));
	}

	TArray<FString> UnusedScripts;
	for (const FString& LuaFile : LuaFiles)
	{
		if (!UsedScripts.Contains(FPaths::GetCleanFilename(LuaFile)))
		{
			UnusedScripts.Add(LuaFile);
		}
	}

	if (UnusedScripts.Num() > 0)
	{
		FString UnusedScriptsList = FString::Join(UnusedScripts, TEXT("\n"));
		FText DialogText = FText::Format(
			LOCTEXT("UnusedScripts", "[Warning] 存在未被使用的脚本:\n{0}"), FText::FromString(UnusedScriptsList));
		FMessageDialog::Open(EAppMsgType::Ok, DialogText);
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("NoUnusedScripts", "更新成功"));
	}
}

static void RegisterGameEditorMenus()
{
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu");
	FToolMenuSection& Section = Menu->AddSection("ProjectJ", TAttribute<FText>(), FToolMenuInsert("Help", EToolMenuInsertType::After));
	Section.AddSubMenu(
		"ProjectJSubMenu",
		LOCTEXT("ProjectJSubMenu_Label", "ProjectJ"),
		LOCTEXT("ProjectJSubMenu_ToolTip", "This is a submenu under ProjectJ"),
		FNewToolMenuDelegate::CreateLambda([](UToolMenu* InMenu)
		{
			FToolMenuSection& SubSection = InMenu->AddSection("ProjectJSubSection");

			// SubSection.AddMenuEntry(
			// 	"ProjectJSubMenuEntry1",
			// 	LOCTEXT("ProjectJSubMenuEntry1_Label", "更新Lua技能描述"),
			// 	LOCTEXT("ProjectJSubMenuEntry1_ToolTip", "目前包括ProjectJGlobal, ProjectJGameplayTags"),
			// 	FSlateIcon(),
			// 	FUIAction(FExecuteAction::CreateStatic(&UpdateLuaAbilityDesc))
			// );

			SubSection.AddMenuEntry(
				"ProjectJSubMenuEntry0",
				LOCTEXT("ProjectJDataTableSubMenuEntry0_Label", "创建行为Lua脚本"),
				LOCTEXT("ProjectJDataTableSubMenuEntry0_ToolTip", "创建行为Lua脚本"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&CreateSpellLuaScript))
			);

			SubSection.AddMenuEntry(
				"ProjectJSubMenuEntry1",
				LOCTEXT("ProjectJDataTableSubMenuEntry1_Label", "创建技能Lua脚本"),
				LOCTEXT("ProjectJDataTableSubMenuEntry1_ToolTip", "创建技能Lua脚本"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&CreateAbilityLuaScript))
			);
			
			SubSection.AddMenuEntry(
				"ProjectJSubMenuEntry2",
				LOCTEXT("ProjectJSubMenuEntry2_Label", "生成静态变量lua"),
				LOCTEXT("ProjectJSubMenuEntry2_ToolTip", "目前包括ProjectJGlobal, ProjectJGameplayTags"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&CreateStaticVariableLua))
			);
			
			// Add entries to the submenu
			SubSection.AddMenuEntry(
				"ProjectJSubMenuEntry3",
				LOCTEXT("ProjectJSubMenuEntry3_Label", "模板迁移"),
				LOCTEXT("ProjectJSubMenuEntry3_ToolTip", "检查脚本函数是否与模板相同(入参, 是否缺少某个函数, 是否多了某个函数)"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&MigrateAbilityTemplate))
			);
		})
	);
	
	// 拓展DataTable工具栏， 创建一个新的菜单
	UToolMenu* DataTableMenu = UToolMenus::Get()->ExtendMenu("AssetEditor.DataTableEditor.MainMenu");
	FToolMenuSection& DataTableSection = DataTableMenu->AddSection("ProjectJDataTable", TAttribute<FText>(), FToolMenuInsert("Help", EToolMenuInsertType::After));
    
	DataTableSection.AddSubMenu(
		"ProjectJDataTableSubMenu",
		LOCTEXT("ProjectJDataTableSubMenu_Label", "ProjectJ DataTable"),
		LOCTEXT("ProjectJDataTableSubMenu_ToolTip", "This is a submenu under ProjectJ DataTable"),
		FNewToolMenuDelegate::CreateLambda([](UToolMenu* InMenu)
		{
			FToolMenuSection& SubSection = InMenu->AddSection("ProjectJDataTableSubSection");

			SubSection.AddMenuEntry(
				"ProjectJDataTableSubMenuEntry1",
				LOCTEXT("ProjectJDataTableSubMenuEntry1_Label", "创建技能Lua脚本"),
				LOCTEXT("ProjectJDataTableSubMenuEntry1_ToolTip", "创建技能Lua脚本"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&CreateAbilityLuaScript))
			);

			SubSection.AddMenuEntry(
				"ProjectJDataTableSubMenuEntry2",
				LOCTEXT("ProjectJDataTableSubMenuEntry2_Label", "更新道具Lua脚本"),
				LOCTEXT("ProjectJDataTableSubMenuEntry2_ToolTip", "更新道具Lua脚本"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&CreatePropLuaScript))
			);
			SubSection.AddMenuEntry(
				"ProjectJDataTableSubMenuEntry2",
				LOCTEXT("ProjectJDataTableSubMenuEntry2_Label", "更新副本Lua脚本"),
				LOCTEXT("ProjectJDataTableSubMenuEntry2_ToolTip", "更新副本Lua脚本"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&CreateDungeonLuaScript))
			);
		})
	);
	// Section.AddMenuEntry(
	// 	"ProjectJTestAction",
	// 	LOCTEXT("ProjectJTestAction", "Test Action"),
	// 	LOCTEXT("ProjectJTestActionTooltip", "This is a test action"),
	// 	FSlateIcon(),
	// 	FUIAction()
	// );
	// Uncomment this to add a custom toolbar that is displayed during PIE
	// Useful for making easy access to changing game state artificially, adding cheats, etc
	// FToolMenuEntry BlueprintEntry = FToolMenuEntry::InitComboButton(
	// 	"ProjectJ",
	// 	FUIAction(
	// 		FExecuteAction()),
	// 	FOnGetContent::CreateStatic(&ProjectJMenu),
	// 	LOCTEXT("ProjectJ_Label", "ProjectJ Options"),
	// 	LOCTEXT("ProjectJ_ToolTip", "ProjectJ Options"),
	// 	FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.OpenLevelBlueprint")
	// );
	// BlueprintEntry.StyleNameOverride = "CalloutToolbar";
	// Section.AddEntry(BlueprintEntry);

	// 有插件实现了更方便的使用
	// FToolMenuEntry CommonMapEntry = FToolMenuEntry::InitComboButton(
	// 	"CommonMapOptions",
	// 	FUIAction(
	// 		FExecuteAction(),
	// 		FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
	// 		FIsActionChecked()),
	// 	FOnGetContent::CreateStatic(&GetCommonMapsDropdown),
	// 	LOCTEXT("CommonMaps_Label", "Common Maps"),
	// 	LOCTEXT("CommonMaps_ToolTip", "Some commonly desired maps while using the editor"),
	// 	FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Level")
	// );
	// CommonMapEntry.StyleNameOverride = "CalloutToolbar";
	// Section.AddEntry(CommonMapEntry);
	
	// UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu(
	// 		"LevelEditor.MainMenu");
	// ToolbarMenu->AddSection(
	// 			   TEXT("ProjectJ"),
	// 			   NSLOCTEXT("ProjectJEditor", "ProjectJHeading", "ProjectJ"),
	// 			   FToolMenuInsert(TEXT("Edit"), EToolMenuInsertType::After));
	//
	// FToolMenuSection& ExpansionSection = ToolbarMenu->FindOrAddSection(TEXT("ProjectJ"));
	//
	// const FUIAction CollapseAllAction( FExecuteAction::CreateStatic(&TestAction, ToolbarMenu) );
	// ExpansionSection.AddMenuEntry(
	// 	TEXT("Test"),
	// 	NSLOCTEXT("PropertyView", "CollapseAll", "Collapse All"),
	// 	NSLOCTEXT("PropertyView", "CollapseAll_ToolTip", "Collapses this item and all children"),
	// 	FSlateIcon(),
	// 	CollapseAllAction);
}

class FProjectJEditorModule : public FDefaultGameModuleImpl
{
	typedef FProjectJEditorModule ThisClass;

	virtual void StartupModule() override
	{
		FProjectJGameEditorStyle::Initialize();
		
		if (!IsRunningGame())
		{
			FModuleManager::Get().OnModulesChanged().AddRaw(this, &FProjectJEditorModule::ModulesChangedCallback);
		
			BindGameplayAbilitiesEditorDelegates();
		
			if (FSlateApplication::IsInitialized())
			{
				ToolMenusHandle = UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateStatic(&RegisterGameEditorMenus));
			}
		
			FEditorDelegates::BeginPIE.AddRaw(this, &ThisClass::OnBeginPIE);
			FEditorDelegates::EndPIE.AddRaw(this, &ThisClass::OnEndPIE);
		}

		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		// PropertyModule.RegisterCustomPropertyTypeLayout("ProjectJLuaLine", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FProjectJLuaLineCustomLayout::MakeInstance));
		// PropertyModule.RegisterCustomPropertyTypeLayout("ProjectJAbilityRow", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FProjectJAbilityRowCustomLayout::MakeInstance));
		// PropertyModule.RegisterCustomPropertyTypeLayout("ProjectJAbilityActionDataConfig", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FProjectJAbilityActionDataConfigCustomLayout::MakeInstance));
		PropertyModule.NotifyCustomizationModuleChanged();

		FToolMenuOwnerScoped OwnerScoped(this);
		
	}

	void OnBeginPIE(bool bIsSimulating)
	{
	
	}

	void OnEndPIE(bool bIsSimulating)
	{
		
	}

	virtual void ShutdownModule() override
	{
		FEditorDelegates::BeginPIE.RemoveAll(this);
		FEditorDelegates::EndPIE.RemoveAll(this);

		if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
		{
			FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
			// PropertyModule.UnregisterCustomPropertyTypeLayout("ProjectJLuaLine");
			// PropertyModule.UnregisterCustomPropertyTypeLayout("ProjectJAbilityRow");
			// PropertyModule.UnregisterCustomPropertyTypeLayout("ProjectJAbilityActionDataConfig");
		}
		
		// Undo UToolMenus
		if (UObjectInitialized() && ToolMenusHandle.IsValid())
		{
			UToolMenus::UnRegisterStartupCallback(ToolMenusHandle);
		}
		
		UnbindGameplayAbilitiesEditorDelegates();
		FModuleManager::Get().OnModulesChanged().RemoveAll(this);
		FProjectJGameEditorStyle::Shutdown();
	}

protected:

	static void BindGameplayAbilitiesEditorDelegates()
	{
		IGameplayAbilitiesEditorModule& GameplayAbilitiesEditorModule = IGameplayAbilitiesEditorModule::Get();
		
		// GameplayAbilitiesEditorModule.GetGameplayCueNotifyClassesDelegate().BindStatic(&GetGameplayCueDefaultClasses);
		// GameplayAbilitiesEditorModule.GetGameplayCueInterfaceClassesDelegate().BindStatic(&GetGameplayCueInterfaceClasses);
		// GameplayAbilitiesEditorModule.GetGameplayCueNotifyPathDelegate().BindStatic(&GetGameplayCuePath);
	}

	static void UnbindGameplayAbilitiesEditorDelegates()
	{
		if (IGameplayAbilitiesEditorModule::IsAvailable())
		{
			IGameplayAbilitiesEditorModule& GameplayAbilitiesEditorModule = IGameplayAbilitiesEditorModule::Get();
			// GameplayAbilitiesEditorModule.GetGameplayCueNotifyClassesDelegate().Unbind();
			// GameplayAbilitiesEditorModule.GetGameplayCueInterfaceClassesDelegate().Unbind();
			// GameplayAbilitiesEditorModule.GetGameplayCueNotifyPathDelegate().Unbind();
		}
	}

	void ModulesChangedCallback(FName ModuleThatChanged, EModuleChangeReason ReasonForChange)
	{
		if ((ReasonForChange == EModuleChangeReason::ModuleLoaded) && (ModuleThatChanged.ToString() == TEXT("GameplayAbilitiesEditor")))
		{
			BindGameplayAbilitiesEditorDelegates();
		}
	}

private:
	FDelegateHandle ToolMenusHandle;
	
private:
	
};
#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FProjectJEditorModule, ProjectJEditor)