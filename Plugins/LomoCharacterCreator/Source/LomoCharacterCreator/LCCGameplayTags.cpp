// Copyright Epic Games, Inc. All Rights Reserved.

#include "LCCGameplayTags.h"

#include "Engine/EngineTypes.h"
#include "GameplayTagsManager.h"
#include "LCCLogChannels.h"

namespace LCCGameplayTags
{
	// Vital Attribute Tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(LCC_Test, "LCC.Test", "Test tag for LomoCharacterCreator.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(LCC_Creator_Base, "LCC.Creator.Base", "Base tag for LomoCharacterCreator.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(LCC_Creator_Base_SkinColor_1, "LCC.Creator.Base.SkinColor.1", "SkinColor 1 tag for LomoCharacterCreator.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(LCC_Creator_Base_SkinColor_2, "LCC.Creator.Base.SkinColor.2", "SkinColor 2 tag for LomoCharacterCreator.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(LCC_Creator_Base_Pose, "LCC.Creator.Base.Pose", "Pose tag for LomoCharacterCreator.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(LCC_Creator_Body, "LCC.Creator.Body", "Body tag for LomoCharacterCreator.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(LCC_Creator_Body_Morphology, "LCC.Creator.Body.Morphology", "Body Morphology tag for LomoCharacterCreator.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(LCC_Creator_Body_Height, "LCC.Creator.Body.Height", "Body Height tag for LomoCharacterCreator.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(LCC_Creator_Hair, "LCC.Creator.Hair", "Hair tag for LomoCharacterCreator.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(LCC_Creator_Hair_Color, "LCC.Creator.Hair.Color", "Hair Color tag for LomoCharacterCreator.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(LCC_Creator_Hair_Style, "LCC.Creator.Hair.Style", "Hair Style tag for LomoCharacterCreator.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(LCC_Creator_Hair_Style_Front, "LCC.Creator.Hair.Style.Front", "Hair Style Front tag for LomoCharacterCreator.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(LCC_Creator_Hair_Style_Back, "LCC.Creator.Hair.Style.Back", "Hair Style Back tag for LomoCharacterCreator.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(LCC_Creator_Hair_Style_Right, "LCC.Creator.Hair.Style.Right", "Hair Style Right tag for LomoCharacterCreator.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(LCC_Creator_Hair_Style_Left, "LCC.Creator.Hair.Style.Left", "Hair Style Left tag for LomoCharacterCreator.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(LCC_Creator_Hair_Tail, "LCC.Creator.Hair.Tail", "Hair Tail tag for LomoCharacterCreator.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(LCC_Creator_Hair_Tail_Style, "LCC.Creator.Hair.Tail.Style", "Hair Tail Style tag for LomoCharacterCreator.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(LCC_Creator_Hair_Tail_Position, "LCC.Creator.Hair.Tail.Position", "Hair Tail Position tag for LomoCharacterCreator.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(LCC_Creator_Head, "LCC.Creator.Head", "Head tag for LomoCharacterCreator.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(LCC_Creator_Head_Eye, "LCC.Creator.Head.Eye", "Head Eye tag for LomoCharacterCreator.");
	

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
					UE_LOG(LogLCC, Display, TEXT("Could not find exact match for tag [%s] but found partial match on tag [%s]."), *TagString, *TestTag.ToString());
					Tag = TestTag;
					break;
				}
			}
		}

		return Tag;
	}
}

