// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UnLuaInterface.h"
#include "Card/ProjectJCharacter.h"
#include "Card/ProjectJItem.h"
#include "Card/ProjectJLandmark.h"
#include "Card/ProjectJSpell.h"
#include "GameFramework/Actor.h"
#include "Types/ProjectJBattleEventData.h"
#include "Types/ProjectJLuaAbilityExecInfo.h"
#include "Types/ProjectJLuaInstanceType.h"
#include "Types/ProjectJTargetFilter.h"
#include "ProjectJLuaExecutor.generated.h"

UCLASS()
class PROJECTJ_API AProjectJLuaExecutor : public AActor, public IUnLuaInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectJLuaExecutor();

	virtual FString GetModuleName_Implementation() const override
	{
		return TEXT("ProjectJLuaExecutor");
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// ----- 关卡玩法相关 Start -----
public:
	UFUNCTION(BlueprintImplementableEvent)
	void CreateDungeon(const FName& RowName, const FName& InLuaScriptName);
	UFUNCTION(BlueprintImplementableEvent)
	void EnterDungeon(const FName& RowName);
	UFUNCTION(BlueprintImplementableEvent)
	void RemoveDungeon(const FName& RowName);

	UFUNCTION(BlueprintImplementableEvent)
	bool CanExecute();

	UFUNCTION(BlueprintImplementableEvent)
	void CreateCharacter(int32 ID, const FName& InLuaScriptName);
	UFUNCTION(BlueprintImplementableEvent)
	void CreateLandMark(int32 ID, const FName& InLuaScriptName);
	UFUNCTION(BlueprintImplementableEvent)
	void CreateUtility(int32 ID, const FName& InLuaScriptName);
	UFUNCTION(BlueprintImplementableEvent)
	void CreateItem(int32 ID, const FName& InLuaScriptName);
	UFUNCTION(BlueprintImplementableEvent)
	void CreateSpell(int32 ID, const FName& InLuaScriptName);
	
	UFUNCTION(BlueprintImplementableEvent)
	void RemoveCharacter(int32 ID);
	UFUNCTION(BlueprintImplementableEvent)
	void RemoveLandMark(int32 ID);
	UFUNCTION(BlueprintImplementableEvent)
	void RemoveItem(int32 ID);
	UFUNCTION(BlueprintImplementableEvent)
	void RemoveUtility(int32 ID);
	UFUNCTION(BlueprintImplementableEvent)
	void RemoveSpell(int32 ID);

	UFUNCTION(BlueprintImplementableEvent)
	FProjectJTargetFilter GetTargetFilter(int32 ID, EProjectJCardType InCardType);
	
	UFUNCTION(BlueprintImplementableEvent)
	void ExecuteSelectTarget(EProjectJCardType CardType, int32 ID, EProjectJCardType SelectedCardType, int32 SelectedID);

	UFUNCTION(BlueprintImplementableEvent)
	int32 GetExecuteMinutes(EProjectJCardType CardType, int32 ID);
	
	UFUNCTION(BlueprintImplementableEvent)
	void ExecuteStart(EProjectJCardType CardType, int32 ID, int32 Frame);

	UFUNCTION(BlueprintImplementableEvent)
	void ExecuteTick(EProjectJCardType CardType, int32 ID, int32 LogicFrame);

	UFUNCTION(BlueprintImplementableEvent)
	void ExecuteOver(EProjectJCardType CardType, int32 ID);

	UFUNCTION(BlueprintImplementableEvent)
	void ExecuteAfterHide(EProjectJCardType CardType, int32 ID);

	UFUNCTION(BlueprintImplementableEvent)
	void OnGetSpellResult(const FName& SpellTag, EProjectJCardType CardType, int32 ID, int32 RollResult);

	// ----- 关卡玩法相关 End -----

	// ----- 战斗技能相关 Start -----
public:
	UFUNCTION(BlueprintImplementableEvent)
	FText GetLuaAbilityDesc(const FName& InLuaScriptName);
	
	UFUNCTION(BlueprintImplementableEvent)
	void CreateLuaAbilityInstance(int32 InOwnerID, int32 InEventID, const FName& InLuaScriptName);
	UFUNCTION(BlueprintImplementableEvent)
	FGameplayTag GetExecTag(int32 InOwnerID, int32 EventID);
	/**
	 * 获取松散特性标签, 用于一些特殊功能实现， 只是简单的给角色添加松散Tag
	 * Todo: 是否需要考虑层数的问题
	 * @param OwnerID
	 * @param EventID
	 * @return 
	 */
	UFUNCTION(BlueprintImplementableEvent)
	TArray<FGameplayTag> GetLooseTag(int32 InOwnerID, int32 EventID);

	UFUNCTION(BlueprintImplementableEvent)
	bool IsTriggerTime(int32 InOwnerID, int32 EventID, const FProjectJBattleEventData& ProjectJEventData);

	UFUNCTION(BlueprintImplementableEvent)
	FProjectJLuaAbilityExecInfo GetLuaAbilityExecInfo(int32 InOwnerID, int EventID, const FProjectJBattleEventData& ProjectJEventData);

	UFUNCTION(BlueprintImplementableEvent)
	void ExecuteLuaAbility(int32 InOwnerID, int32 EventID, const FProjectJBattleEventData& ProjectJEventData, const TArray<int32>& InTargets);

	UFUNCTION(BlueprintImplementableEvent)
	void RemoveLuaAbilityInstance(int32 InOwnerID, int32 EventID);
	// ----- 战斗技能相关 End -----
};
