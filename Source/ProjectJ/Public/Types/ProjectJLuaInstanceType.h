#pragma once
#include "CoreMinimal.h"

#include "ProjectJLuaInstanceType.generated.h"

UENUM(BlueprintType)
enum class EProjectJLuaInstanceType : uint8
{
	Level = 0,
	Character = 1,
	Landmark = 2,
};
