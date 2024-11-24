// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/DeveloperSettings/ProjectJDataTableSettings.h"

FName UProjectJDataTableSettings::GetCategoryName() const
{
	return FApp::GetProjectName();
}
