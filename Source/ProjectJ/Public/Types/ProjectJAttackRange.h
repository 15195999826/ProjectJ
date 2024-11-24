#pragma once
#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EProjectJAttackRange : uint8
{
	Closet UMETA(DisplayName = "最近目标"),
	Front UMETA(DisplayName = "最前目标"),
	Back UMETA(DisplayName = "最后目标"),
	AOE UMETA(DisplayName = "敌方全体"),
	RandomOne UMETA(DisplayName = "随机单个目标"),
};