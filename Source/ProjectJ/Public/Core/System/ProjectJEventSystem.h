// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectJ/ProjectJDelegates.h"
#include "Subsystems/WorldSubsystem.h"
#include "Types/ProjectJGameStage.h"
#include "Types/ProjectJLevelConfig.h"
#include "Types/ProjectJRecord.h"
#include "Types/Item/ProjectJItemBase.h"
#include "ProjectJEventSystem.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FProjectJNameLevelConfigDelegate, const FName&, const FProjectJLevelConfig&);
DECLARE_MULTICAST_DELEGATE_OneParam(FProjectJOneCardDelegate, class AProjectJCardBase*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FProjectJOneStageChangeDelegate, EProjectJGameStage, EProjectJGameStage);
DECLARE_MULTICAST_DELEGATE_TwoParams(FProjectJPostLuaAbilityStatusDelegate, int32 /* CharacterID */, bool /* IsRunning */);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FProjectJStoryRecordSignature, const FProjectJStoryRecord&, Record);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FProjectJFocusEquipmentSlotSignature, bool , IsFocus,const FName& , RowName, EProjectJItemType, ItemType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FProjectJFocusFeatureBoxSignature, bool, IsFocus, const FGameplayTag&, FeatureTag);
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
	FProjectJOneNameDelegate OnCreateUtility;
	FProjectJNameLevelConfigDelegate OnSaveLevel;
	// ----------------- 编辑器事件 End-----------------

	// ----------------- 表演事件 Start-----------------
	UPROPERTY(BlueprintCallable)
	FProjectJStoryRecordSignature PostStoryRecord;
	
	FSimpleMulticastDelegate OnPerformEnd;
	// ----------------- 表演事件 End-----------------

	// ----------------- UI事件 Start-----------------
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FProjectJFocusEquipmentSlotSignature OnFocusEquipmentSlot;
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FProjectJFocusFeatureBoxSignature OnFocusFeatureBox;
	// ----------------- UI事件 End-----------------

	
	FProjectJOneStageChangeDelegate OnStageChange;
	FProjectJOneCardDelegate OnObserveCard;
	FProjectJOneCardDelegate OnCheckCard;
	FProjectJOneCardDelegate OnHideAtCard;
	// 偷袭
	FProjectJOneCardDelegate OnAmbushCard;


	// ----------------- 战斗事件 Start-----------------
	FOnProjectJOneIntParamDelegate OnWaitingAttack;
	FOnProjectJOneIntParamDelegate OnAttackHit;
	FOnProjectJOneIntParamDelegate AfterAttackHit;
	FOnProjectJOneIntParamDelegate OnIdleReturnToPosition;
	FProjectJPostLuaAbilityStatusDelegate PostLuaAbilityStatus;
	
	// -----------------战斗事件 End-----------------
};
