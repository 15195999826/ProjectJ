// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AttributeSet.h"
#include "NativeGameplayTags.h"

namespace ProjectJGameplayTags
{
	PROJECTJ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Test)
	
	PROJECTJ_API FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);
	// Debug Tags
	PROJECTJ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Debug_Buff);

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

	// 战斗特性Tag
	// 松散特性Tag， 用于快速实现一些逻辑功能，无表现, 也不会抛出事件
	PROJECTJ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Feature_Loose_OneDamageEachTime); // 每次只受到1点伤害
	
	// 特性Tag
	PROJECTJ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Feature_None); // 无特性
	PROJECTJ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Feature_Stun); // 眩晕
	PROJECTJ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Feature_Taunt); // 嘲讽
	PROJECTJ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Feature_Poison); // 中毒
	
	PROJECTJ_API extern const TArray<FGameplayTag> BadFeatureTags;
	PROJECTJ_API extern const TArray<FGameplayTag> NeutralFeatureTags;
	PROJECTJ_API extern const TArray<FGameplayTag> GoodFeatureTags;
	PROJECTJ_API extern const TArray<FGameplayTag> LooseFeatureTags;
	

	// 战斗事件
	PROJECTJ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Battle_Event_None);
	PROJECTJ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Battle_Event_RoundStart);
	PROJECTJ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Battle_Event_RoundEnd);
	
	PROJECTJ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Battle_Event_BeforeAttack);
	PROJECTJ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Battle_Event_AfterAttack);
	
	PROJECTJ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Battle_Event_GetDamage);
	PROJECTJ_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Battle_Event_Dead);
	
	PROJECTJ_API extern const TArray<FGameplayTag> BattleEventPak;
};
