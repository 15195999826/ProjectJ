// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GAS/ProjectJLuaGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "Core/DeveloperSettings/ProjectJGeneralSettings.h"
#include "Core/DeveloperSettings/ProjectJPropertyHelper.h"
#include "Core/System/ProjectJContextSystem.h"
#include "Game/ProjectJGameBFL.h"
#include "Game/ProjectJLuaExecutor.h"
#include "Game/Card/ProjectJCharacter.h"
#include "Game/GAS/ProjectJGameplayEffectContext.h"
#include "ProjectJ/ProjectJGameplayTags.h"
#include "ProjectJ/ProjectJLogChannels.h"
#include "Types/ProjectJValid.h"

int32 UProjectJLuaGameplayAbility::RegisterAbility(const FName& InLuaScriptCHSName)
{
	auto EventID = SelfEventID++;
	auto Owner = Cast<AProjectJCharacter>(GetAvatarActorFromActorInfo());
	// 从Lua脚本中获取， 当前技能的触发条件
	auto LuaAbilityExecutor = GetWorld()->GetSubsystem<UProjectJContextSystem>()->LuaExecutor;
	const auto& LuaScriptsMap = GetDefault<UProjectJPropertyHelper>()->CHS2AbilityLuaScriptNameMap;
	if (!LuaScriptsMap.Contains(InLuaScriptCHSName))
	{
		UE_LOG(LogProjectJ, Error, TEXT("RegisterAbility 失败: %s, 配置字典中没有对应的lua文件"), *InLuaScriptCHSName.ToString());
		return -1;
	}
	LuaAbilityExecutor->CreateLuaAbilityInstance(Owner->ID, EventID, LuaScriptsMap[InLuaScriptCHSName]);
	FGameplayTag ExecTag = LuaAbilityExecutor->GetExecTag(Owner->ID, EventID);
	auto& EventArray = GameEvents.FindOrAdd(ExecTag);
	EventArray.Add(EventID);
	
	const auto& LooseTags = LuaAbilityExecutor->GetLooseTag(Owner->ID, EventID);
	auto ASC = GetAbilitySystemComponentFromActorInfo();
	for (const auto& Tag : LooseTags)
	{
		ASC->AddLooseGameplayTag(Tag);
	}
	return EventID;
}

void UProjectJLuaGameplayAbility::RemoveBattleEvent(int32 InEventID)
{
	
}

void UProjectJLuaGameplayAbility::ResetAbility()
{
	auto ASC = GetAbilitySystemComponentFromActorInfo();
	for (auto& Pair : GameEvents)
	{
		for (auto& EventID : Pair.Value)
		{
			auto Owner = Cast<AProjectJCharacter>(GetAvatarActorFromActorInfo());
			auto LuaAbilityExecutor = GetWorld()->GetSubsystem<UProjectJContextSystem>()->LuaExecutor;
			
			const auto& LooseTags = LuaAbilityExecutor->GetLooseTag(Owner->ID, EventID);
			for (const auto& Tag : LooseTags)
			{
				ASC->RemoveLooseGameplayTag(Tag);
			}
			LuaAbilityExecutor->RemoveLuaAbilityInstance(Owner->ID, EventID);
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
	const auto& ProjectJEventData = UProjectJGameBFL::TryGetProjectJGameEventData(InBattleEventPayload, Valid);
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
