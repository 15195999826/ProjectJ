// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Card/ProjectJUtility.h"

#include "Components/TextRenderComponent.h"
#include "Game/ProjectJGameBFL.h"

AProjectJUtility::AProjectJUtility()
{
	PrimaryActorTick.bCanEverTick = false;

	FloatWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("FloatWidgetComponent"));
	FloatWidgetComponent->SetupAttachment(RootComponent);
}

void AProjectJUtility::BindConfig_Implementation(const FName& InRowName)
{
	ConfigRowName = InRowName;
	auto ConfigRow = UProjectJGameBFL::GetUtilityConfig(ConfigRowName);
	if (ConfigRow)
	{
		Title->SetText(ConfigRow->Name);
	}
	else
	{
		Title->SetText(FText::FromString(TEXT("未知类型")));
	}
}

FName AProjectJUtility::GetConfigRowName_Implementation()
{
	return ConfigRowName;
}
