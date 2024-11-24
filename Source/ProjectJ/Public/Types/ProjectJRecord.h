#pragma once

#include "CoreMinimal.h"

#include "ProjectJRecord.generated.h"

UENUM()
enum class EProjectJPerformType {
	None,
	Story, // 剧情
};


USTRUCT(BlueprintType)
struct FProjectJStoryRecord
{
	GENERATED_BODY()

	FProjectJStoryRecord(){}

	UPROPERTY(BlueprintReadOnly)
	TArray<FText> Story;
};
