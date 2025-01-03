#pragma once

namespace ProjectJGlobal
{
	// 定义全局常量
	// Shop ScenePoint
	PROJECTJ_API extern const FName Test;

	PROJECTJ_API extern const TArray<FName> RightTeams;
	PROJECTJ_API extern const TMap<FName, int32> LeftTeamToPosition;

	// ------BattleEventData Keys Start------
	PROJECTJ_API extern const FName Battle_DamageValueKey;
	PROJECTJ_API extern const FName Battle_HealValueKey;
	PROJECTJ_API extern const FName Battle_GetFeatureKey;
	PROJECTJ_API extern const FName Battle_GetFeatureRoundKey;
	PROJECTJ_API extern const FName Battle_GetFeatureLayerCountKey;

	// ------BattleEventData Keys End------
	PROJECTJ_API extern const FName Program_Ani_NameKey;
	PROJECTJ_API extern const FName Program_Ani_LocationKey;

	PROJECTJ_API extern const TArray<FName> AnimCustomKeys;

	PROJECTJ_API extern const FName Program_Ani_Knock;

	// 对应的值为int类型，然后转化成Enum
	PROJECTJ_API extern const FName EffectAttachTypeKey;

	// ----Spell Tags Start----
	PROJECTJ_API extern const FName GuanCha;
	PROJECTJ_API extern const FName YinBi;
	PROJECTJ_API extern const FName TouXi;
	PROJECTJ_API extern const FName TouQie;

	// Todo: 未来其它角色的特殊行为

	// ----Spell Tags End----

}
