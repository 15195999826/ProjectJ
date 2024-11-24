// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/ProjectJHUD.h"

#include "EngineUtils.h"
#include "Game/Card/ProjectJCardBase.h"

void AProjectJHUD::GetDebugActorList(TArray<AActor*>& InOutList)
{
	Super::GetDebugActorList(InOutList);
	UWorld* World = GetWorld();

	// find CardBase
	for (TActorIterator<AProjectJCardBase> It(World); It; ++It)
	{
		AddActorToDebugList(*It, InOutList, World);
	}
}
