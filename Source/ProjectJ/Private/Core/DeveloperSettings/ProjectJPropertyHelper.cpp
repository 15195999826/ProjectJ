// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/DeveloperSettings/ProjectJPropertyHelper.h"
#include "ProjectJ/ProjectJDWGlobal.h"
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
