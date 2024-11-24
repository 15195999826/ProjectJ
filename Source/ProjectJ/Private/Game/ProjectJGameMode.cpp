// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ProjectJGameMode.h"

#include "EngineUtils.h"
#include "Core/ProjectJGameInstance.h"
#include "Core/DeveloperSettings/ProjectJGeneralSettings.h"
#include "Core/System/ProjectJContextSystem.h"
#include "Game/ProjectJGameProgress.h"
#include "Game/ProjectJLevelSettingActor.h"
#include "Game/ProjectJSceneUIManager.h"
#include "Game/ProjectJLuaExecutor.h"
#include "Game/ProjectJPerformManager.h"
#include "Game/ProjectJBattleManager.h"

void AProjectJGameMode::BeginPlay()
{
	auto GI = GetWorld()->GetGameInstance<UProjectJGameInstance>();
	GI->ProtectInitUIRoot();

	auto GeneralSettings = GetDefault<UProjectJGeneralSettings>();
	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	ContextSystem->GameProgress = GetWorld()->SpawnActor<AProjectJGameProgress>(GeneralSettings->GameProgressClass);

	// 查询场景中的LevelSettingActor
	for (TActorIterator<AProjectJLevelSettingActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ContextSystem->LevelSettingActor = *ActorItr;
		break;
	}

	for (TActorIterator<AProjectJSceneUIManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ContextSystem->SceneUIManager = *ActorItr;
		break;
	}
	
	for (TActorIterator<AProjectJLuaExecutor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ContextSystem->LuaExecutor = *ActorItr;
		break;
	}

	for (TActorIterator<AProjectJPerformManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ContextSystem->PerformManager = *ActorItr;
		break;
	}

	for (TActorIterator<AProjectJBattleManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ContextSystem->BattleManager = *ActorItr;
		break;
	}
	Super::BeginPlay();

	// FString FilePath = FPaths::SourceConfigDir() + "/Config/Unlua_LocText.ini";
	// FText CustomValue;
	// if (GConfig->GetText(TEXT("ProjectJ"), TEXT("Test"), CustomValue, FilePath))
	// {
	// 	UE_LOG(LogTemp, Log, TEXT("测试ini本地化 value: %s"), *CustomValue.ToString());
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Failed to read CustomField from GameUserSettings.ini"));
	// }
}

void AProjectJGameMode::StartPlay()
{
	Super::StartPlay();
	
	// 所有Actor已加载完毕
	OnLevelPrepared.Broadcast();
}
