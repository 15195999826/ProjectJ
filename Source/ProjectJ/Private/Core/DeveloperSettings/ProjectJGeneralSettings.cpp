// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/DeveloperSettings/ProjectJGeneralSettings.h"

FName UProjectJGeneralSettings::GetCategoryName() const
{
	return FApp::GetProjectName();
}
