// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildLevel/ProjectJBuildLevelGameMode.h"

#include "EngineUtils.h"
#include "BuildLevel/ProjectJBuildLevelTipsActor.h"
#include "Core/System/ProjectJContextSystem.h"
#include "Game/ProjectJCardLayoutManager.h"
#include "Game/ProjectJLevelSettingActor.h"

void AProjectJBuildLevelGameMode::BeginPlay()
{
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	
	// 查询场景中的LevelSettingActor
	for (TActorIterator<AProjectJLevelSettingActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ContextSystem->LevelSettingActor = *ActorItr;
		break;
	}

	for (TActorIterator<AProjectJCardLayoutManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ContextSystem->CardLayoutManager = *ActorItr;
		break;
	}
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
