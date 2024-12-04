#pragma once
#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EProjectJTargetTeam : uint8
{
	Teammate UMETA(DisplayName = "队友"),
	Enemy UMETA(DisplayName = "敌人"),
	Any UMETA(DisplayName = "任意"),
};

UENUM(BlueprintType)
enum class EProjectJAttackRange : uint8
{
	Closet UMETA(DisplayName = "最近目标"),
	Front UMETA(DisplayName = "最前目标"),
	Back UMETA(DisplayName = "最后目标"),
	AOE UMETA(DisplayName = "全体"),
	RandomOne UMETA(DisplayName = "随机单个目标"),
};