#pragma once
#include "CoreMinimal.h"
#include "ProjectJDungeonConfig.generated.h"

USTRUCT(BlueprintType)
struct FProjectJDungeonSpawnCondition
{
	GENERATED_BODY()

	FProjectJDungeonSpawnCondition(): Day(0)
	{
	}

	// Todo: 临时使用
	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta=(DisplayName="指定天刷新"))
	int32 Day;
};

/**
 * 副本
 */
USTRUCT(BlueprintType)
struct FProjectJDungeonConfig : public FTableRowBase
{
	GENERATED_BODY()

	FProjectJDungeonConfig()
		:Description(NAME_None)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Description;

	// Todo: 满足X条件， 刷新该副本
	// Todo: 刷新所在地点
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="刷新地点", GetOptions="ProjectJ.ProjectJGeneralSettings.GetPointNames"))
	FName Point;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="刷新条件"))
	FProjectJDungeonSpawnCondition SpawnCondition;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName LuaScriptName;
};

