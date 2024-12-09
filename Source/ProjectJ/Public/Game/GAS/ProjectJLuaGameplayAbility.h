// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ProjectJLuaGameplayAbility.generated.h"

/**
 * 角色技能
 */
UCLASS()
class PROJECTJ_API UProjectJLuaGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	int32 RegisterAbility(const FName& InLuaScriptCHSName);
	/**
	 * 特性的功能实现
	 * 1. 给与特性标签
	 * 2. 检查是否存在对应的Lua脚本, 存在则RegisterAbility， 并将EventID额外记录到CachedFeatureEventIDs
	 * 3. 区分LooseFeatureTags和FeatureGE的原因是:
	 *	  -有些纯逻辑的功能， 非外显的词条Tag， 无需对应的GE， 简化功能制作流程
	 *	  -特性Tag则总有一个对应的GE， 是游戏内定义好的词条
	 * @param InFeatureTag
	 * @param InCount
	 * @param InRound 持续回合数， -1表示永久
	 */
	void GiveFeature(const FGameplayTag& InFeatureTag, int32 InCount, int32 InRound);
	
	void RemoveBattleEvent(int32 InEventID);
	void ResetAbility();

protected:
	int32 SelfEventID = 0;
	
	TMap<FGameplayTag, TArray<int32>> GameEvents;
	TMap<FGameplayTag, int32> CachedFeatureEventIDs;
	
	UFUNCTION(BlueprintCallable)
	void ReceiveBattleEvent(const FGameplayEventData& InBattleEventPayload);
};
