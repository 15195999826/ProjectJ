// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GAS/ProjectJAbilitySystemComponent.h"

void UProjectJAbilitySystemComponent::HandleMontagePostEvent(FGameplayTag EventTag, FProjectJMontageEventData EventData)
{
	if (FProjectJMontagePostDelegate* Delegate = MontageEventCallbacks.Find(EventTag))
	{
		// Make a copy before broadcasting to prevent memory stomping
		FProjectJMontagePostDelegate DelegateCopy = *Delegate;
		DelegateCopy.Broadcast(&EventData);
	}
}
