// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GAS/Montage/ProjectJMontagePostTagAbilityTask.h"

UProjectJMontagePostTagAbilityTask* UProjectJMontagePostTagAbilityTask::CreateMontagePost(UGameplayAbility* OwningAbility,
	FGameplayTag EventTag, bool OnlyTriggerOnce)
{
	UProjectJMontagePostTagAbilityTask* RetTask = NewAbilityTask<UProjectJMontagePostTagAbilityTask>(OwningAbility);
	RetTask->Tag = EventTag;
	RetTask->OnlyTriggerOnce = OnlyTriggerOnce;
	return RetTask;
}

void UProjectJMontagePostTagAbilityTask::Activate()
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
		&UProjectJMontagePostTagAbilityTask::MontagePostCallback);
}

void UProjectJMontagePostTagAbilityTask::OnDestroy(bool bInOwnerFinished)
{
	auto ProjectJASC = Cast<UProjectJAbilitySystemComponent>(AbilitySystemComponent);
	if (ProjectJASC && MyHandle.IsValid())
	{
		ProjectJASC->MontageEventCallbacks.FindOrAdd(Tag).Remove(MyHandle);
	}
		
	Super::OnDestroy(bInOwnerFinished);
}


void UProjectJMontagePostTagAbilityTask::MontagePostCallback(FProjectJMontageEventData* PanguMontageEventData)
{
	EventReceived.Broadcast(*PanguMontageEventData);
		
	if (OnlyTriggerOnce)
	{
		EndTask();
	}
}
