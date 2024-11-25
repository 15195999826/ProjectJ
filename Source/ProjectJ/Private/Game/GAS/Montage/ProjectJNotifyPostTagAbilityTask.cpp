// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GAS/Montage/ProjectJNotifyPostTagAbilityTask.h"

UProjectJNotifyPostTagAbilityTask* UProjectJNotifyPostTagAbilityTask::CreateNotifyPost(UGameplayAbility* OwningAbility,
	FGameplayTag EventTag, bool OnlyTriggerOnce)
{
	UProjectJNotifyPostTagAbilityTask* RetTask = NewAbilityTask<UProjectJNotifyPostTagAbilityTask>(OwningAbility);
	RetTask->Tag = EventTag;
	RetTask->OnlyTriggerOnce = OnlyTriggerOnce;
	return RetTask;
}

void UProjectJNotifyPostTagAbilityTask::Activate()
{
	Super::Activate();

	auto ProjectJASC = Cast<UProjectJAbilitySystemComponent>(AbilitySystemComponent);
	if (ProjectJASC == nullptr)
	{
		EndTask();
		TaskFailed.Broadcast();
		return;
	}

	MyHandle = ProjectJASC->MontageEventCallbacks.FindOrAdd(Tag).AddUObject(this,
		&UProjectJNotifyPostTagAbilityTask::MontagePostCallback);
}

void UProjectJNotifyPostTagAbilityTask::OnDestroy(bool bInOwnerFinished)
{
	auto ProjectJASC = Cast<UProjectJAbilitySystemComponent>(AbilitySystemComponent);
	if (ProjectJASC && MyHandle.IsValid())
	{
		ProjectJASC->MontageEventCallbacks.FindOrAdd(Tag).Remove(MyHandle);
	}
		
	Super::OnDestroy(bInOwnerFinished);
}


void UProjectJNotifyPostTagAbilityTask::MontagePostCallback(FProjectJMontageEventData* PanguMontageEventData)
{
	EventReceived.Broadcast(*PanguMontageEventData);
		
	if (OnlyTriggerOnce)
	{
		EndTask();
	}
}
