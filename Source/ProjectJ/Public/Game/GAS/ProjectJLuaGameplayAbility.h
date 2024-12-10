// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Types/Item/ProjectJEquipmentConfig.h"
#include "ProjectJLuaGameplayAbility.generated.h"



USTRUCT(BlueprintType)
struct FProjectJLuaAbilityAnimation
{
	GENERATED_BODY()
	
	FProjectJLuaAbilityAnimation()
	{
		
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="动画类型"))
	EProjectJAbilityAnimationType AnimationType = EProjectJAbilityAnimationType::Program;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="资源", EditConditionHides, EditCondition="AnimationType != EProjectJAbilityAnimationType::Program"))
	TSubclassOf<UObject> Resource;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="自定义字段(暂时没用)",GetKeyOptions="ProjectJ.ProjectJPropertyHelper.GetAnimCustomKeys"))
	TMap<FName, FString> CustomKV; 
};

USTRUCT(BlueprintType)
struct FProjectJLuaAbilityExecInfo
{
	GENERATED_BODY()

	FProjectJLuaAbilityExecInfo()
	{
		
	}
	
	UPROPERTY()
	TArray<FProjectJLuaAbilityAnimation> Animations;

	// 触发功能的时间点
	UPROPERTY()
	float TriggerPoint = 0.f;

	UPROPERTY()
	
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

protected:
	int32 SelfEventID = 0;
	
	TMap<FGameplayTag, TArray<int32>> GameEvents;
	
	UFUNCTION(BlueprintCallable)
	void ReceiveBattleEvent(const FGameplayEventData& InBattleEventPayload);
};
