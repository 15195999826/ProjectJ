#pragma once
#include "CoreMinimal.h"

#include "ProjectJGameStage.generated.h"

UENUM(BlueprintType)
enum class EProjectJGameStage : uint8
{
	None = 0,
	Idle = 1,
	Observing = 2,
	// Todo:
	Checking = 3,
	Hiding = 4,
	Ambushing = 5,

	Performing = 6,
};