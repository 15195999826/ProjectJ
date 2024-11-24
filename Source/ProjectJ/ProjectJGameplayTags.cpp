// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectJGameplayTags.h"

#include "Engine/EngineTypes.h"
#include "GameplayTagsManager.h"
#include "ProjectJLogChannels.h"
#define LOCTEXT_NAMESPACE "ProjectJGameplayTags"
namespace ProjectJGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Test, "Test", "Test gameplay tag.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_Battle_Health, "SetByCaller.Attribute.Battle.Health", "生命值");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_Battle_Attack, "SetByCaller.Attribute.Battle.Attack", "攻击力");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_Battle_Speed, "SetByCaller.Attribute.Battle.Speed", "身法"); 
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_Battle_Armor, "SetByCaller.Attribute.Battle.Armor", "护甲");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Attribute_Battle_Damage, "SetByCaller.Attribute.Battle.Damage", "伤害");
	
	const TArray<FGameplayTag> RuntimeAttributes = {
		
	};

	const TMap<FGameplayTag, FText> Attributes2Text ={
		// {SetByCaller_Attribute_Player_Gold, LOCTEXT("Gold", "金币")},
		// {SetByCaller_Attribute_Player_Round, LOCTEXT("Round", "回合")},
		// {SetByCaller_Attribute_Player_WinTimes, LOCTEXT("WinTimes", "胜利次数")},
		// {SetByCaller_Attribute_Player_MaxWinTimes, LOCTEXT("MaxWinTimes", "最大胜利次数")},
		// {SetByCaller_Attribute_Player_LeftLife, LOCTEXT("LeftLife", "剩余生命")},
		// {SetByCaller_Attribute_Player_Difficulty, LOCTEXT("Difficulty", "难度")},
	};

	const TMap<FGameplayTag, TFunction<FGameplayAttribute()>> Tag2Attribute = {
		// {SetByCaller_Attribute_Player_Gold, [](){return UAutoDWPlayerAttributeSet::GetGoldAttribute();}},
		// {SetByCaller_Attribute_Player_Round, [](){return UAutoDWPlayerAttributeSet::GetRoundAttribute();}},
		// {SetByCaller_Attribute_Player_WinTimes, [](){return UAutoDWPlayerAttributeSet::GetWinTimesAttribute();}},
		// {SetByCaller_Attribute_Player_MaxWinTimes, [](){return UAutoDWPlayerAttributeSet::GetMaxWinTimesAttribute();}},
		// {SetByCaller_Attribute_Player_LeftLife, [](){return UAutoDWPlayerAttributeSet::GetLeftLifeAttribute();}},
		// {SetByCaller_Attribute_Player_Difficulty, [](){return UAutoDWPlayerAttributeSet::GetDifficultyAttribute();}},
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

