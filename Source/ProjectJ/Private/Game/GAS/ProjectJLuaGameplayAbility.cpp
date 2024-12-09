// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GAS/ProjectJLuaGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "Core/DeveloperSettings/ProjectJPropertyHelper.h"
#include "Core/System/ProjectJContextSystem.h"
#include "Game/ProjectJGameBPFL.h"
#include "Game/ProjectJLuaExecutor.h"
#include "Game/Card/ProjectJCharacter.h"
#include "ProjectJ/ProjectJGameplayTags.h"
#include "ProjectJ/ProjectJLogChannels.h"
#include "Types/ProjectJValid.h"

int32 UProjectJLuaGameplayAbility::RegisterAbility(const FName& InLuaScriptCHSName)
{
	auto EventID = SelfEventID++;
	auto Owner = Cast<AProjectJCharacter>(GetAvatarActorFromActorInfo());
	// 从Lua脚本中获取， 当前技能的触发条件
	auto LuaAbilityExecutor = GetWorld()->GetSubsystem<UProjectJContextSystem>()->LuaExecutor;
	const auto& Map = GetDefault<UProjectJPropertyHelper>()->CHS2AbilityLuaScriptNameMap;
	if (!Map.Contains(InLuaScriptCHSName))
	{
		UE_LOG(LogProjectJ, Error, TEXT("RegisterAbility 失败: %s, 配置字典中没有对应的lua文件"), *InLuaScriptCHSName.ToString());
		return -1;
	}
	LuaAbilityExecutor->CreateLuaAbilityInstance(Owner->ID, EventID, Map[InLuaScriptCHSName]);
	FGameplayTag ExecTag = LuaAbilityExecutor->GetExecTag(Owner->ID, EventID);
	auto& EventArray = GameEvents.FindOrAdd(ExecTag);
	EventArray.Add(EventID);
	
	const auto& FeatureTags = LuaAbilityExecutor->GetFeatureTag(Owner->ID, EventID);
	for (const auto& Tag : FeatureTags)
	{
		if (ProjectJGameplayTags::LooseFeatureTags.Contains(Tag))
		{
			GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(Tag);
		}
		else
		{
			// 给与对应的GE
			
		}
	}
	
	// if (LooseFeatureTag.IsValid())
	// {
	// 	GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(LooseFeatureTag);
	// }
	
	return EventID;
}

void UProjectJLuaGameplayAbility::RemoveBattleEvent(int32 InEventID)
{
	
}

void UProjectJLuaGameplayAbility::ResetAbility()
{
	for (auto& Pair : GameEvents)
	{
		for (auto& EventID : Pair.Value)
		{
			// auto Owner = Cast<AProjectJCharacter>(GetAvatarActorFromActorInfo());
			// auto LuaAbilityExecutor = GetWorld()->GetSubsystem<UProjectJContextSystem>()->LuaAbilityExecutor;
			//
			// FGameplayTag LooseFeatureTag = LuaAbilityExecutor->GetLooseFeatureTag(Owner->ID, EventID);
			// if (LooseFeatureTag.IsValid())
			// {
			// 	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(LooseFeatureTag);
			// }
			// LuaAbilityExecutor->RemoveLuaAbilityInstance(Owner->ID, EventID);
		}
	}

	GameEvents.Empty();
}

void UProjectJLuaGameplayAbility::ReceiveBattleEvent(const FGameplayEventData& InBattleEventPayload)
{
	auto Owner = Cast<AProjectJCharacter>(GetAvatarActorFromActorInfo());
	auto InTag = InBattleEventPayload.EventTag;

	// UE_LOG(LogProjectJ, Error, TEXT("ReceiveBattleEvent: %s, ID:%d"), *InTag.ToString(), Owner->ID);
	EProjectJValid Valid = EProjectJValid::Invalid;
	const auto& ProjectJEventData = UProjectJGameBPFL::TryGetProjectJGameEventData(InBattleEventPayload, Valid);
	// 确保总是传递了非空的数据的
	check(Valid == EProjectJValid::Valid);
	if (auto Pair = GameEvents.Find(InTag))
	{
		for (auto& EventID : *Pair)
		{
			auto LuaAbilityExecutor = GetWorld()->GetSubsystem<UProjectJContextSystem>()->LuaExecutor;
			UE_LOG(LogProjectJ, Error, TEXT("ExecuteLua: %s, ID:%d"), *InTag.ToString(), Owner->ID);
			LuaAbilityExecutor->ExecuteLuaAbility(Owner->ID, EventID, ProjectJEventData);
		}
	}
}
