#pragma once
#include "CoreMinimal.h"
#include "ProjectJDungeonConfig.generated.h"

/**
 * 副本
 */
USTRUCT(BlueprintType)
struct FProjectJDungeonConfig : public FTableRowBase
{
	GENERATED_BODY()

	FProjectJDungeonConfig()
		: Name(NAME_None)
		, Description(NAME_None)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Description;

	// Todo: 满足X条件， 刷新该副本
	// Todo: 刷新所在地点
};

