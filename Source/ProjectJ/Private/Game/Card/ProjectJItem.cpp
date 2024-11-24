// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Card/ProjectJItem.h"

#include "Components/WidgetComponent.h"

AProjectJItem::AProjectJItem()
{
	PrimaryActorTick.bCanEverTick = false;

	FloatWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("FloatWidgetComponent"));
	FloatWidgetComponent->SetupAttachment(RootComponent);
}

void AProjectJItem::BindConfig_Implementation(const FName& InRowName)
{
	IProjectJCardInterface::BindConfig_Implementation(InRowName);
}

FName AProjectJItem::GetConfigRowName_Implementation()
{
	return IProjectJCardInterface::GetConfigRowName_Implementation();
}
