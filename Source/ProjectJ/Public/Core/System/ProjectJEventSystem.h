// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectJ/ProjectJDelegates.h"
#include "Subsystems/WorldSubsystem.h"
#include "Types/ProjectJGameStage.h"
#include "Types/ProjectJLevelConfig.h"
#include "Types/ProjectJRecord.h"
#include "ProjectJEventSystem.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FProjectJNameLevelConfigDelegate, const FName&, const FProjectJLevelConfig&);
DECLARE_MULTICAST_DELEGATE_OneParam(FProjectJOneCardDelegate, class AProjectJCardBase*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FProjectJOneStageChangeDelegate, EProjectJGameStage, EProjectJGameStage);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FProjectJStoryRecordSignature, const FProjectJStoryRecord&, Record);
/**
 * 
 */
UCLASS()
class PROJECTJ_API UProjectJEventSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// ----------------- 编辑器事件 Start-----------------
	FProjectJOneNameDelegate OnCreateLevel;
	FProjectJOneNameDelegate OnCreateCharacter;
	FProjectJOneNameDelegate OnCreateLandmark;
	FProjectJNameLevelConfigDelegate OnSaveLevel;
	// ----------------- 编辑器事件 End-----------------

	// ----------------- 表演事件 Start-----------------
	UPROPERTY(BlueprintCallable)
	FProjectJStoryRecordSignature PostStoryRecord;
	
	FSimpleMulticastDelegate OnPerformEnd;
	// ----------------- 表演事件 End-----------------
	
	FProjectJOneStageChangeDelegate OnStageChange;
	FProjectJOneCardDelegate OnObserveCard;
	FProjectJOneCardDelegate OnCheckCard;
	FProjectJOneCardDelegate OnHideAtCard;
	// 偷袭
	FProjectJOneCardDelegate OnAmbushCard;
};
