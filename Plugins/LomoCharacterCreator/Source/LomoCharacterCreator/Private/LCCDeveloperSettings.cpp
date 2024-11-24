// Fill out your copyright notice in the Description page of Project Settings.


#include "LCCDeveloperSettings.h"

FName ULCCDeveloperSettings::GetCategoryName() const
{
	// 写打印项目名字
	
	
	return FApp::GetProjectName();
}
