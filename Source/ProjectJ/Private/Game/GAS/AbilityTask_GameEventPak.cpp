// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GAS/AbilityTask_GameEventPak.h"

#include "AbilitySystemComponent.h"
#include "ProjectJ/ProjectJGameplayTags.h"

UAbilityTask_GameEventPak* UAbilityTask_GameEventPak::ProjectJGameEventPak(UGameplayAbility* OwningAbility)
{
	UAbilityTask_GameEventPak* MyObj = NewAbilityTask<UAbilityTask_GameEventPak>(OwningAbility);
	return MyObj;
}

void UAbilityTask_GameEventPak::Activate()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC)
	{
		for (const auto& Tag : ProjectJGameplayTags::BattleEventPak)
		{
			auto Handle = ASC->GenericGameplayEventCallbacks.FindOrAdd(Tag).AddLambda([this, Tag](const FGameplayEventData* Payload)
			{
				GameplayEventCallback(Tag, Payload);
			});
			GameEventHandleMap.Add(Tag, Handle);
		}
	}

	
	Super::Activate();
}

void UAbilityTask_GameEventPak::OnDestroy(bool bInOwnerFinished)
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC)
	{
		for (auto& Pair : GameEventHandleMap)
		{
			if (Pair.Value.IsValid())
			{
				ASC->GenericGameplayEventCallbacks.FindOrAdd(Pair.Key).Remove(Pair.Value);
			}
		}
	}
	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_GameEventPak::GameplayEventCallback(FGameplayTag MatchingTag, const FGameplayEventData* Payload) const
{
	ensureMsgf(Payload, TEXT("GameplayEventCallback expected non-null Payload"));
	FGameplayEventData TempPayload = Payload ? *Payload : FGameplayEventData{};
	TempPayload.EventTag = MatchingTag;
	EventReceived.Broadcast(MoveTemp(TempPayload));
}
