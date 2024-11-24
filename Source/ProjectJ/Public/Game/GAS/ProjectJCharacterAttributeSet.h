// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeAccessor.h"
#include "AttributeSet.h"
#include "ProjectJCharacterAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTJ_API UProjectJCharacterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	// 观察
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Observe;
	ATTRIBUTE_ACCESSORS(UProjectJCharacterAttributeSet, Observe);

	// 感知
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Perception;
	ATTRIBUTE_ACCESSORS(UProjectJCharacterAttributeSet, Perception);

	// ----- 战斗属性 Start-----
	// 攻击力
	UPROPERTY(BlueprintReadOnly, Category= "BattleAttribute|Vital")
	FGameplayAttributeData Attack;
	ATTRIBUTE_ACCESSORS(UProjectJCharacterAttributeSet, Attack);

	// 生命值
	UPROPERTY(BlueprintReadOnly, Category= "BattleAttribute|Vital")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UProjectJCharacterAttributeSet, Health);

	// 身法
	UPROPERTY(BlueprintReadOnly, Category= "BattleAttribute|Vital")
	FGameplayAttributeData Speed;
	ATTRIBUTE_ACCESSORS(UProjectJCharacterAttributeSet, Speed);

	// 护甲值 Todo: 可以做护甲，战斗时总是恢复到满， HP则不会
	UPROPERTY(BlueprintReadOnly, Category= "BattleAttribute|Meta")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UProjectJCharacterAttributeSet, Armor);

	// 受伤
	UPROPERTY(BlueprintReadOnly, Category= "BattleAttribute|Meta")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UProjectJCharacterAttributeSet, Damage);

	UPROPERTY(BlueprintReadOnly, Category= "BattleAttribute|Meta")
	FGameplayAttributeData Team;
	ATTRIBUTE_ACCESSORS(UProjectJCharacterAttributeSet, Team);

	UPROPERTY(BlueprintReadOnly, Category= "BattleAttribute|Meta")
	FGameplayAttributeData Position;
	ATTRIBUTE_ACCESSORS(UProjectJCharacterAttributeSet, Position);
	
	// ----- 战斗属性 End-----
};
