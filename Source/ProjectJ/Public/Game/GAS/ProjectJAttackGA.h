// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/GAS/ProjectJGameplayAbility.h"
#include "Types/Item/ProjectJEquipmentConfig.h"
#include "ProjectJAttackGA.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTJ_API UProjectJAttackGA : public UProjectJGameplayAbility
{
	GENERATED_BODY()

public:
	void SetAttackConfig(const FProjectJAttackAbility& InAttackAbility);
};
