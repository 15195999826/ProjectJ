// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectJGameplayTags.h"

#include "Engine/EngineTypes.h"
#include "GameplayTagsManager.h"
#include "ProjectJLogChannels.h"
#include "Game/GAS/ProjectJCharacterAttributeSet.h"
#define LOCTEXT_NAMESPACE "ProjectJGameplayTags"
namespace ProjectJGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Test, "Test", "Test gameplay tag.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debug_Buff, "Debug.Buff", "Debug Buff");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActionPoint0, "Ability.ActionPoint0", "Action Point 0");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActionPoint1, "Ability.ActionPoint1", "Action Point 1");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActionPoint2, "Ability.ActionPoint2", "Action Point 2");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActionPoint3, "Ability.ActionPoint3", "Action Point 3");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActionPoint4, "Ability.ActionPoint4", "Action Point 4");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActionPoint5, "Ability.ActionPoint5", "Action Point 5");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_Battle_Health, "SetByCaller.Attribute.Battle.Health", "生命值");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_Battle_Attack, "SetByCaller.Attribute.Battle.Attack", "攻击力");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_Battle_Speed, "SetByCaller.Attribute.Battle.Speed", "身法"); 
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_Battle_Armor, "SetByCaller.Attribute.Battle.Armor", "护甲");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_Battle_Damage, "SetByCaller.Attribute.Battle.Damage", "伤害");
	
	const TArray<FGameplayTag> RuntimeAttributes = {
		
	};

	const TMap<FGameplayTag, FText> Attributes2Text ={
		{SetByCaller_Attribute_Battle_Health, LOCTEXT("Health", "生命值")},
		{SetByCaller_Attribute_Battle_Attack, LOCTEXT("Attack", "攻击力")},
		{SetByCaller_Attribute_Battle_Speed, LOCTEXT("Speed", "身法")},
		{SetByCaller_Attribute_Battle_Armor, LOCTEXT("Armor", "护甲")},
		{SetByCaller_Attribute_Battle_Damage, LOCTEXT("Damage", "伤害")},
	};

	const TMap<FGameplayTag, TFunction<FGameplayAttribute()>> Tag2Attribute = {
		{SetByCaller_Attribute_Battle_Health, [](){return UProjectJCharacterAttributeSet::GetHealthAttribute();}},
		{SetByCaller_Attribute_Battle_Attack, [](){return UProjectJCharacterAttributeSet::GetAttackAttribute();}},
		{SetByCaller_Attribute_Battle_Speed, [](){return UProjectJCharacterAttributeSet::GetSpeedAttribute();}},
		{SetByCaller_Attribute_Battle_Armor, [](){return UProjectJCharacterAttributeSet::GetArmorAttribute();}},
		{SetByCaller_Attribute_Battle_Damage, [](){return UProjectJCharacterAttributeSet::GetDamageAttribute();}},
	};
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Feature_Loose_OneDamageEachTime, "Feature.Loose.OneDamageEachTime", "Loose one damage each time.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Feature_None, "Feature.None", "No feature.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Feature_Stun, "Feature.Stun", "Stun feature.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Feature_Taunt, "Feature.Taunt", "Taunt feature.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Feature_Poison, "Feature.Poison", "Poison feature.");
	
	const TArray<FGameplayTag> BadFeatureTags = {
		Feature_Stun,
		Feature_Poison
	};

	const TArray<FGameplayTag> NeutralFeatureTags = {
		Feature_Taunt
	};

	const TArray<FGameplayTag> GoodFeatureTags = {
		
	};

	const TArray<FGameplayTag> LooseFeatureTags = {
		Feature_Loose_OneDamageEachTime
	};

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Battle_Event_None, "Battle.Event.None", "None");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Battle_Event_RoundStart, "Battle.Event.RoundStart", "Round Start");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Battle_Event_RoundEnd, "Battle.Event.RoundEnd", "Round End");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Battle_Event_BeforeAttack, "Battle.Event.BeforeAttack", "Before Attack");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Battle_Event_AfterAttack, "Battle.Event.AfterAttack", "After Attack");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Battle_Event_GetDamage, "Battle.Event.GetDamage", "Get Damage");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Battle_Event_GetHeal, "Battle.Event.GetHeal", "Get Heal");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Battle_Event_Dead, "Battle.Event.Dead", "Dead");

	const TArray<FGameplayTag> BattleEventPak = {
		Battle_Event_RoundStart,
		Battle_Event_RoundEnd,
		Battle_Event_BeforeAttack,
		Battle_Event_AfterAttack,
		Battle_Event_GetDamage,
		Battle_Event_GetHeal,
		Battle_Event_Dead,
	};
	
	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString)
	{
		const UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
		FGameplayTag Tag = Manager.RequestGameplayTag(FName(*TagString), false);

		if (!Tag.IsValid() && bMatchPartialString)
		{
			FGameplayTagContainer AllTags;
			Manager.RequestAllGameplayTags(AllTags, true);

			for (const FGameplayTag& TestTag : AllTags)
			{
				if (TestTag.ToString().Contains(TagString))
				{
					UE_LOG(LogProjectJ, Display, TEXT("Could not find exact match for tag [%s] but found partial match on tag [%s]."), *TagString, *TestTag.ToString());
					Tag = TestTag;
					break;
				}
			}
		}

		return Tag;
	}
}

#undef LOCTEXT_NAMESPACE

