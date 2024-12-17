// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Card/ProjectJCardExecuteArea.h"

void AProjectJCardExecuteArea::BindConfig_Implementation(const FName& InRowName)
{
	IProjectJCardInterface::BindConfig_Implementation(InRowName);
}

FName AProjectJCardExecuteArea::GetConfigRowName_Implementation()
{
	return IProjectJCardInterface::GetConfigRowName_Implementation();
}
