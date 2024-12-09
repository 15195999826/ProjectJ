// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GAS/ProjectJAbilitySystemGlobals.h"
#include "Game/GAS/ProjectJGameplayEffectContext.h"

FGameplayEffectContext* UProjectJAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FProjectJGameplayEffectContext();
}
