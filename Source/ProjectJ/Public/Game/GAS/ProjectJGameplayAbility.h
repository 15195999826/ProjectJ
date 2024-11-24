// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ProjectJGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTJ_API UProjectJGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UProjectJGameplayAbility()
	{
		InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	}
};
