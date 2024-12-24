// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/DeveloperSettings/ProjectJGeneralSettings.h"

FName UProjectJGeneralSettings::GetCategoryName() const
{
	return FApp::GetProjectName();
}

TArray<FName> UProjectJGeneralSettings::GetPointNames()
{
	auto DefaultTZ = GetDefault<UProjectJGeneralSettings>();
	TArray<FName> Names;
	for (const auto& Point : DefaultTZ->MapPoints)
	{
		for (const auto& AreaPoint : Point.AreaPointNames)
		{
			auto Combine = Point.Name.ToString() + TEXT("_") + AreaPoint.ToString();
			Names.Add(FName(*Combine));
		}
	}

	return Names;
}
