// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AttributeSet.h"
#include "NativeGameplayTags.h"

namespace ProjectJGameplayTags
{
	PROJECTJ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Test)
	
	PROJECTJ_API FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);

	// ActionPoints, 用于Montage动画抛出事件触发点
	PROJECTJ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActionPoint0);
	PROJECTJ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActionPoint1);
	PROJECTJ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActionPoint2);
	PROJECTJ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActionPoint3);
	PROJECTJ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActionPoint4);
	PROJECTJ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActionPoint5);

	// 战斗属性
	PROJECTJ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Battle_Health);
	PROJECTJ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Battle_Attack);
	PROJECTJ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Battle_Speed);
	PROJECTJ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Battle_Armor);
	PROJECTJ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Attribute_Battle_Damage);
	
	PROJECTJ_API extern const TArray<FGameplayTag> RuntimeAttributes;
	PROJECTJ_API extern const TMap<FGameplayTag, FText> Attributes2Text;
	PROJECTJ_API extern const TMap<FGameplayTag, TFunction<FGameplayAttribute()>> Tag2Attribute;
	

	
};
