// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Types/ProjectJBattleEventData.h"
#include "Types/ProjectJLuaAbilityExecInfo.h"
#include "ProjectJLuaGameplayAbility.generated.h"

class AProjectJEffectActor;

USTRUCT()
struct FProjectJLuaAbilityExecInfoCache
{
	GENERATED_BODY()

	int32 EventID;

	UPROPERTY()
	FProjectJLuaAbilityExecInfo ExecInfo;
};

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

	bool HasEventAtTag(const FGameplayTag& InTag);

protected:
	int32 SelfEventID = 0;
	
	TMap<FGameplayTag, TArray<int32>> GameEvents;

	UFUNCTION(BlueprintCallable)
	void ReceiveBattleEvent(const FGameplayEventData& InBattleEventPayload);

	UPROPERTY()
	FProjectJBattleEventData CacheProjectJEventData;
	UPROPERTY()
	TArray<FProjectJLuaAbilityExecInfoCache> CachedExecInfos;
	int32 CurrentExecInfoIndex = 0;

	void NextExec();
	void TriggerExec();
	FTimerHandle ExecOverTimerHandle;
	FTimerHandle ExecTriggerTimerHandle;

private:
	static UAnimMontage* LoadMontageFromString(const FString& MontageSoftPath);
};
