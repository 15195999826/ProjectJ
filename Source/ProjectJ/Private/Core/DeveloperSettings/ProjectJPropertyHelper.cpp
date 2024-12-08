// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/DeveloperSettings/ProjectJPropertyHelper.h"
#include "ProjectJ/ProjectJDWGlobal.h"

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
