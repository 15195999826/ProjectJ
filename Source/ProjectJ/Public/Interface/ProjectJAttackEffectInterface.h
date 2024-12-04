// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ProjectJAttackEffectInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UProjectJAttackEffectInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTJ_API IProjectJAttackEffectInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	 * 获取攻击点, StartAttack后经过多久造成伤害, 约定不做多段伤害
	 * @return 攻击点
	 */
	UFUNCTION(BlueprintImplementableEvent)
	float GetAttackPoint();
};
