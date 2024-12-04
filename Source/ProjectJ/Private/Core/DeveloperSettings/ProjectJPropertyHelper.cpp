// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/DeveloperSettings/ProjectJPropertyHelper.h"

#include "Core/DeveloperSettings/ProjectJDataTableSettings.h"
#include "ProjectJ/ProjectJDWGlobal.h"

TArray<FName> UProjectJPropertyHelper::GetAnimCustomKeys()
{
	return ProjectJGlobal::AnimCustomKeys;
}
