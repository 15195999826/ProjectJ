#pragma once
#include "CoreMinimal.h"

#include "ProjectJLuaInstanceType.generated.h"

UENUM(BlueprintType)
enum class EProjectJLuaInstanceType : uint8
{
	Level = 0,
	Character = 1,
	Landmark = 2,
	Ability = 3,
	Prop = 4, // 道具: 主要用于实现道具在场景中的执行功能
};
