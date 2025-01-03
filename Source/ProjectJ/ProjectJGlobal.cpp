#include "ProjectJGlobal.h"

namespace ProjectJGlobal
{
	const FName Test = TEXT("Test");
	
	const TArray<FName> LeftTeams = {
	};


	const TMap<FName, int32> LeftTeamToPosition = {

	};

	// ------BattleEventData Keys Start------
	const FName Battle_DamageValueKey = TEXT("Battle_DamageValue");
	const FName Battle_HealValueKey = TEXT("Battle_HealValue");
	const FName Battle_GetFeatureKey = TEXT("Battle_GetFeature");
	const FName Battle_GetFeatureRoundKey = TEXT("Battle_GetFeatureRound");
	const FName Battle_GetFeatureLayerCountKey = TEXT("Battle_GetFeatureLayerCount");

	// ------BattleEventData Keys End------

	const FName Program_Ani_NameKey = TEXT("Program_Ani_Name");
	const FName Program_Ani_LocationKey = TEXT("Location");

	const TArray<FName> AnimCustomKeys = {
		Program_Ani_NameKey
	};

	const FName Program_Ani_Knock = TEXT("Program_Ani_Knock");

	// ----Spell Tags Start----

	const FName GuanCha = TEXT("SpellGuanCha");
	const FName YinBi = TEXT("SpellYinBi");
	const FName TouXi = TEXT("SpellTouXi");
	const FName TouQie = TEXT("SpellTouQie");
	
	// ----Spell Tags End----
}
