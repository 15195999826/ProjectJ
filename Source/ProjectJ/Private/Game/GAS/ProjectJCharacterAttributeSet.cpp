// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GAS/ProjectJCharacterAttributeSet.h"

void UProjectJCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute == GetDamageAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0, FLT_MAX);
	}
}

void UProjectJCharacterAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
}

void UProjectJCharacterAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue,
	float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}

void UProjectJCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}
