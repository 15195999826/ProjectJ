// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildLevel/ProjectJBuildLevelGameMode.h"

#include "BuildLevel/ProjectJBuildLevelTipsActor.h"
#include "Interface/ProjectJDragInterface.h"

void AProjectJBuildLevelGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectJBuildLevelGameMode::StartPlay()
{
	Super::StartPlay();
	OnLevelPrepared.Broadcast();
}

void AProjectJBuildLevelGameMode::AttachTips(AActor* InActor, FVector TipsOffset)
{
	// 生成一个TipsActor, Attach到NewActor上
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.OverrideLevel = GetLevel();
	SpawnParams.ObjectFlags |= RF_Transient;

	AProjectJBuildLevelTipsActor* TipsActor = GetWorld()->SpawnActor<AProjectJBuildLevelTipsActor>(
		TipsActorClass, SpawnParams);
	TipsActor->AttachToActor(InActor, FAttachmentTransformRules::KeepRelativeTransform);
	TipsActor->SetActorRelativeLocation(TipsOffset);
	TipsActors.Add(InActor->GetUniqueID(), TipsActor);
}

void AProjectJBuildLevelGameMode::DetachTips(AActor* InActor)
{
	auto UniqueID = InActor->GetUniqueID();
	if (TipsActors.Contains(UniqueID))
	{
		TipsActors[UniqueID]->Destroy();
		TipsActors.Remove(UniqueID);
	}
}
