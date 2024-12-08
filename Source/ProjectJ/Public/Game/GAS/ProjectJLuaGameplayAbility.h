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

	void RemoveBattleEvent(int32 InEventID);
	void ResetAbility();

protected:
	int32 SelfEventID = 0;
	
	TMap<FGameplayTag, TArray<int32>> GameEvents;
	
	UFUNCTION(BlueprintCallable)
	void ReceiveBattleEvent(const FGameplayEventData& InBattleEventPayload);
};
