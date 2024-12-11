// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GAS/ProjectJLuaGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "Core/DeveloperSettings/ProjectJGeneralSettings.h"
#include "Core/DeveloperSettings/ProjectJPropertyHelper.h"
#include "Core/System/ProjectJContextSystem.h"
#include "Core/System/ProjectJEventSystem.h"
#include "Game/ProjectJEffectActor.h"
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

bool UProjectJLuaGameplayAbility::HasEventAtTag(const FGameplayTag& InTag)
{
	return GameEvents.Contains(InTag);
}

void UProjectJLuaGameplayAbility::ReceiveBattleEvent(const FGameplayEventData& InBattleEventPayload)
{
	auto Owner = Cast<AProjectJCharacter>(GetAvatarActorFromActorInfo());
	auto InTag = InBattleEventPayload.EventTag;

	// UE_LOG(LogProjectJ, Error, TEXT("ReceiveBattleEvent: %s, ID:%d"), *InTag.ToString(), Owner->ID);
	EProjectJValid Valid = EProjectJValid::Invalid;
	CacheProjectJEventData = UProjectJGameBFL::TryGetProjectJGameEventData(InBattleEventPayload, Valid);
	// 确保总是传递了非空的数据的
	check(Valid == EProjectJValid::Valid);

	check(CachedExecInfos.Num() == 0);
	if (auto Pair = GameEvents.Find(InTag))
	{
		for (auto& EventID : *Pair)
		{
			auto LuaAbilityExecutor = GetWorld()->GetSubsystem<UProjectJContextSystem>()->LuaExecutor;
			UE_LOG(LogProjectJ, Error, TEXT("ExecuteLua: %s, ID:%d"), *InTag.ToString(), Owner->ID);

			if (LuaAbilityExecutor->IsTriggerTime(Owner->ID, EventID, CacheProjectJEventData))
			{
				FProjectJLuaAbilityExecInfoCache Cache;
				Cache.EventID = EventID;
				Cache.ExecInfo = LuaAbilityExecutor->GetLuaAbilityExecInfo(Owner->ID, EventID, CacheProjectJEventData);
				CachedExecInfos.Add(Cache);
			}
		}
	}

	if (CachedExecInfos.Num() > 0)
	{
		// 通知技能触发
		auto EventSystem = GetWorld()->GetSubsystem<UProjectJEventSystem>();
		EventSystem->PostLuaAbilityStatus.Broadcast(Owner->ID, true);

		CurrentExecInfoIndex = -1;
		// 开始执行技能
		NextExec();
	}

	// Todo: 执行完毕后， 通过EventSystem通知外部， 技能执行完毕了

	// 根据ExecInfos， 播放动画， 然后在TriggerPoint事件点， 调用LuaAbilityExecutor->ExecuteLuaAbility(Owner->ID, EventID, ProjectJEventData)造成技能效果
	
}

void UProjectJLuaGameplayAbility::NextExec()
{
	GetWorld()->GetTimerManager().ClearTimer(ExecOverTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(ExecTriggerTimerHandle);
	CurrentExecInfoIndex++;
	if (CurrentExecInfoIndex >= CachedExecInfos.Num())
	{
		// 执行完毕
		auto EventSystem = GetWorld()->GetSubsystem<UProjectJEventSystem>();
		EventSystem->PostLuaAbilityStatus.Broadcast(Cast<AProjectJCharacter>(GetAvatarActorFromActorInfo())->ID, false);
		return;
	}
	
	const auto& Cache = CachedExecInfos[CurrentExecInfoIndex];

	auto ContextSystem = GetWorld()->GetSubsystem<UProjectJContextSystem>();
	// 根据数据播放动画
	for (const auto& Anim : Cache.ExecInfo.Animations)
	{
		auto Target = ContextSystem->UsingCharacters[Anim.TargetID];
		check(Target.IsValid());

		switch (Anim.AnimationType)
		{
			case EProjectJAbilityAnimationType::Program:
				{
					// Todo: 
				}
				break;
			case EProjectJAbilityAnimationType::Effect:
				{
					auto EffectActorClass = LoadEffectActorFromString(Anim.ResourceSoftPath)->GetClass();
					auto EffectActor = ContextSystem->GetEffectActor(EffectActorClass);
					// Todo: Attach To Target, 等放置方式
					EffectActor->SetActorLocation(Target->GetActorLocation());
					EffectActor->StartEffect();
				}
				break;
			case EProjectJAbilityAnimationType::Montage:
				{
					// String To Montage
					auto Montage = LoadMontageFromString(Anim.ResourceSoftPath);
					if (Montage)
					{
						auto Mesh = Cast<AProjectJCharacter>(GetAvatarActorFromActorInfo())->Mesh;
						Mesh->GetAnimInstance()->Montage_Play(Montage);
					}
				}
				break;
		}
	}
	
	GetWorld()->GetTimerManager().SetTimer(
		ExecOverTimerHandle,
		this,
		&UProjectJLuaGameplayAbility::NextExec,
		Cache.ExecInfo.Duration,
		true
	);
	GetWorld()->GetTimerManager().SetTimer(
		ExecTriggerTimerHandle,
		this,
		&UProjectJLuaGameplayAbility::TriggerExec,
		Cache.ExecInfo.Duration,
		true
	);
}

void UProjectJLuaGameplayAbility::TriggerExec()
{
	auto Owner = Cast<AProjectJCharacter>(GetAvatarActorFromActorInfo());
	auto LuaAbilityExecutor = GetWorld()->GetSubsystem<UProjectJContextSystem>()->LuaExecutor;
	auto CurrentCache = CachedExecInfos[CurrentExecInfoIndex];
	LuaAbilityExecutor->ExecuteLuaAbility(Owner->ID, CurrentCache.EventID, CacheProjectJEventData, CurrentCache.ExecInfo.TargetIDs);
}

UAnimMontage* UProjectJLuaGameplayAbility::LoadMontageFromString(const FString& MontageSoftPath)
{
	if (MontageSoftPath.IsEmpty())
	{
		return nullptr;
	}
	
	FSoftObjectPath SoftObjectPath(MontageSoftPath);
	
	UAnimMontage* Montage = Cast<UAnimMontage>(SoftObjectPath.TryLoad());
	if (Montage == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Load Montage Failed: %s"), *MontageSoftPath);
	}
	return Montage;
}

AProjectJEffectActor* UProjectJLuaGameplayAbility::LoadEffectActorFromString(const FString& EffectSoftPath)
{
	if (EffectSoftPath.IsEmpty())
	{
		return nullptr;
	}
	
	FSoftObjectPath SoftObjectPath(EffectSoftPath);
	
	AProjectJEffectActor* EffectActor = Cast<AProjectJEffectActor>(SoftObjectPath.TryLoad());
	if (EffectActor == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Load EffectActor Failed: %s"), *EffectSoftPath);
	}
	return EffectActor;
}
