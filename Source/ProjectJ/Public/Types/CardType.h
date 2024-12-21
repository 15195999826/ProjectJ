#pragma once

#include "CoreMinimal.h"

#include "CardType.generated.h"

UENUM(BlueprintType)
enum class EProjectJCardType: uint8
{
	None,
	// 角色
	Character,
	// 施法
	Spell,
	// 地标
	Landmark,
	// 物品
	Item
};
