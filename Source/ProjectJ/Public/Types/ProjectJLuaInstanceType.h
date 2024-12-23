#pragma once
#include "CoreMinimal.h"

#include "ProjectJLuaInstanceType.generated.h"

// 因为它跟卡牌类型还是有区别的， 比如技能和关卡， 不是卡牌类型
UENUM(BlueprintType)
enum class EProjectJLuaInstanceType : uint8
{
	Level = 0,
	Character = 1,
	Landmark = 2,
	Ability = 3,
	Prop = 4, 
	Utility = 5,// 道具: 主要用于实现道具在场景中的执行功能
};
