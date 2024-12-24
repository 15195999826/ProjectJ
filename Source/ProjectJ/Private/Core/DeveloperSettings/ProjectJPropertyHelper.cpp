// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/DeveloperSettings/ProjectJPropertyHelper.h"

#include "Core/DeveloperSettings/ProjectJDataTableSettings.h"
#include "ProjectJ/ProjectJGlobal.h"
#include "ProjectJ/ProjectJGameplayTags.h"

TArray<FName> UProjectJPropertyHelper::GetAnimCustomKeys()
{
	return ProjectJGlobal::AnimCustomKeys;
}

TArray<FName> UProjectJPropertyHelper::GetLuaAbilityScriptsNames()
{
	auto DefaultTZ = GetDefault<UProjectJPropertyHelper>();
	TArray<FName> Keys;
	DefaultTZ->CHS2AbilityLuaScriptNameMap.GetKeys(Keys);
	return Keys;
}

TArray<FName> UProjectJPropertyHelper::GetFeatureNames()
{
	TArray<FName> Ret;
	for (const auto& Tag : ProjectJGameplayTags::BadFeatureTags)
	{
		Ret.Add(Tag.GetTagName());
	}

	for (const auto& Tag : ProjectJGameplayTags::GoodFeatureTags)
	{
		Ret.Add(Tag.GetTagName());
	}

	for (const auto& Tag : ProjectJGameplayTags::NeutralFeatureTags)
	{
		Ret.Add(Tag.GetTagName());
	}

	return Ret;
}

TArray<FName> UProjectJPropertyHelper::GetCharacters()
{
	auto CharacterTable = GetDefault<UProjectJDataTableSettings>()->CharacterTable.LoadSynchronous();
	return CharacterTable->GetRowNames();
}

TArray<FName> UProjectJPropertyHelper::GetLandmarks()
{
	auto LandmarkTable = GetDefault<UProjectJDataTableSettings>()->LandmarkTable.LoadSynchronous();
	return LandmarkTable->GetRowNames();
}

TArray<FName> UProjectJPropertyHelper::GetUtilities()
{
	auto UtilityTable = GetDefault<UProjectJDataTableSettings>()->UtilityTable.LoadSynchronous();
	return UtilityTable->GetRowNames();
}
