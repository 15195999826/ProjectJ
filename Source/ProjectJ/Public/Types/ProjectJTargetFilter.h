#pragma once
#include "CoreMinimal.h"
#include "CardType.h"
#include "ProjectJTargetFilter.generated.h"

USTRUCT(BlueprintType)
struct FProjectJTargetFilter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EProjectJCardType> TargetTypes;
};
