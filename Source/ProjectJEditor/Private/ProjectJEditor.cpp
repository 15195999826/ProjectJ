﻿#include "ProjectJEditor.h"

#include "BuildLevelWorldSubsystem.h"
#include "ProjectJGameEditorStyle.h"
#include "ProjectJLuaFunction.h"
#include "GameplayAbilitiesEditorModule.h"
#include "PythonBridge.h"
#include "Editor/UnrealEdEngine.h"
#include "Engine/GameInstance.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "ToolMenu.h"
#include "ToolMenus.h"
#include "Core/DeveloperSettings/ProjectJDataTableSettings.h"
#include "UObject/UObjectIterator.h"

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

static void IntervalMigrateAbilityTemplate(EProjectJLuaInstanceType InType)
{
	const char* TemplateFilePath;
	FString GAbilityLuaSrcRelativePath;
	switch (InType) {
		case EProjectJLuaInstanceType::Level:
			TemplateFilePath = "Config/LuaTemplates/LevelTemplate.lua";
			GAbilityLuaSrcRelativePath = TEXT("Script/Levels/");
			break;
		case EProjectJLuaInstanceType::Character:
			TemplateFilePath = "Config/LuaTemplates/CharacterTemplate.lua";
			GAbilityLuaSrcRelativePath = TEXT("Script/Characters/");
			break;
		case EProjectJLuaInstanceType::Landmark:
			TemplateFilePath = "Config/LuaTemplates/LandmarkTemplate.lua";
			GAbilityLuaSrcRelativePath = TEXT("Script/Landmarks/");
			break;
		default:
			return;
	}
	
	auto FullFilePath = FPaths::ProjectConfigDir() / TemplateFilePath;
	FString AbilityTemplateContent;
	FFileHelper::LoadFileToString(AbilityTemplateContent, *FullFilePath);

	// 将每一个function提取为FProjectJLuaFunction结构， 每一个函数的Comment保存在函数名上方---[[[与---]]]之间
	TArray<FProjectJLuaFunction> TemplateFunctions;
	FRegexPattern FunctionPattern(TEXT("function\\s+M:(\\w+)\\(([^)]*)\\)"));
	FRegexPattern CommentPattern(TEXT("(?s)---\\[\\[\\[(.*?)---\\]\\]\\]"));
	FRegexMatcher FunctionMatcher(FunctionPattern, AbilityTemplateContent);
	FRegexMatcher CommentMatcher(CommentPattern, AbilityTemplateContent);
	
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
		FRegexMatcher FunctionContentMatcher(FunctionContentPattern, AbilityTemplateContent);
	
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
					if (TemplateFunction.Parameters != LuaFunction.Parameters || TemplateFunction.Comment != LuaFunction
						.Comment)
					{
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

							const FProjectJLuaFunction& LuaFunction = LuaFunctions[i];
							LuaContent = LuaContent.Replace(*LuaFunction.Comment, *TemplateFunction.Comment);
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
	auto LevelTableVariables = MakeTableRowValues(DTSettings->LevelTable.LoadSynchronous(), TEXT("Level"));
	auto CharacterTableVariables = MakeTableRowValues(DTSettings->CharacterTable.LoadSynchronous(), TEXT("Character"));
	auto LandmarkTableVariables = MakeTableRowValues(DTSettings->LandmarkTable.LoadSynchronous(), TEXT("Landmark"));

	// LevelTableVariables\CharacterTableVariables\LandmarkTableVariables 三者都需要写入Content
	Content = FString::Join(LevelTableVariables, TEXT("\n"));
	Content = Content + TEXT("\n") + FString::Join(CharacterTableVariables, TEXT("\n"));
	Content = Content + TEXT("\n") + FString::Join(LandmarkTableVariables, TEXT("\n"));
	
	TableContent = TableContent.Replace(TEXT("---{Content}"), *Content);
	auto TablePath = FString::Printf(TEXT("%sTableRows.lua"), *GLuaSrcFullPath);
	FFileHelper::SaveStringToFile(TableContent, *TablePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
}

static void MigrateAbilityTemplate()
{
	IntervalMigrateAbilityTemplate(EProjectJLuaInstanceType::Level);
	IntervalMigrateAbilityTemplate(EProjectJLuaInstanceType::Character);
	IntervalMigrateAbilityTemplate(EProjectJLuaInstanceType::Landmark);
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