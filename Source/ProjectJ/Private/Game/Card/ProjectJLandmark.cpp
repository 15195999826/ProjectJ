// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Card/ProjectJLandmark.h"

#include "Components/TextRenderComponent.h"

void AProjectJLandmark::BindConfig_Implementation(const FName& InRowName)
{
	ConfigRowName = InRowName;
	Title->SetText(FText::FromString(InRowName.ToString()));
}

FName AProjectJLandmark::GetConfigRowName_Implementation()
{
	return ConfigRowName;
}
