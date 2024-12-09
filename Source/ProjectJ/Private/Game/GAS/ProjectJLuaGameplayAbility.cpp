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
	
	// const auto& FeatureTags = LuaAbilityExecutor->GetFeatureTag(Owner->ID, EventID);
	// for (const auto& Tuple : FeatureTags)
	// {
	// 	GiveFeature(Tuple.Key, Tuple.Value);
	// }
	return EventID;
}

void UProjectJLuaGameplayAbility::GiveFeature(const FGameplayTag& InFeatureTag, int32 InCount, int32 InRound)
{
	// 特性的功能必须是唯一的， 比如已经存在嘲讽特性， 那么就不能再添加嘲讽特性
	// Todo: 如果我要给一个角色赋予X回合的嘲讽， 该怎么实现呢？
	// Todo: 如果获得一个3回合的嘲讽，和一个4回合的嘲讽， 又该怎么办呢？
	const auto& LuaScriptsMap = GetDefault<UProjectJPropertyHelper>()->CHS2AbilityLuaScriptNameMap;
	auto ASC = GetAbilitySystemComponentFromActorInfo();
	// Todo: LooseTag 反复添加多层， 是否有必要， 它会用来做带层数的复杂功能吗？ 目前先不做限制
	if (ProjectJGameplayTags::LooseFeatureTags.Contains(InFeatureTag))
	{
		ASC->AddLooseGameplayTag(InFeatureTag, InCount);
	}
	else
	{
		// 给与对应的GE
		const auto& FeatureGE = GetDefault<UProjectJGeneralSettings>()->FeatureGEMap;
		if (FeatureGE.Contains(InFeatureTag))
		{
			// Todo: 特性暂定全是自己赋予自己的
			const auto& Handle = UProjectJGameBFL::SimpleMakeGESpecHandle(GetAvatarActorFromActorInfo(), FeatureGE[InFeatureTag]);
			auto GEContext = static_cast<FProjectJGameplayEffectContext*>(Handle.Data->GetContext().Get());
			GEContext->SetDuration(InRound);
			for (int i = 0; i < InCount; i++)
			{
				ASC->ApplyGameplayEffectSpecToSelf(*Handle.Data.Get());
			}
		}
		else
		{
			UE_LOG(LogProjectJ, Error, TEXT("注册特性失败: %s, 配置字典中没有对应的GE"), *InFeatureTag.ToString());
			return;
		}
	}

	// 检查是否需要赋予对应的特性Lua技能
	if (!CachedFeatureEventIDs.Contains(InFeatureTag))
	{
		const auto& TagName = InFeatureTag.GetTagName();
		// 将.替换成_
		auto LuaScriptName = FName(*TagName.ToString().Replace(TEXT("."), TEXT("_")));
		if (LuaScriptsMap.Contains(LuaScriptName))
		{
			auto EventID = RegisterAbility(LuaScriptName);
			CachedFeatureEventIDs.Add(InFeatureTag, EventID);
		}
	}
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
			// auto LuaAbilityExecutor = GetWorld()->GetSubsystem<UProjectJContextSystem>()->LuaExecutor;
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
