#pragma once
#include "CoreMinimal.h"
#include "ProjectJLevelConfig.generated.h"

USTRUCT(BlueprintType)
struct FProjectJNavPoint
{
	GENERATED_BODY()
	
	FProjectJNavPoint(){}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ToLevel;
};

USTRUCT(BlueprintType)
struct FProjectJLevelCardPoint
{
	GENERATED_BODY()
	
	FProjectJLevelCardPoint(){}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName RowName;

	// Todo: 出现条件等配置
	
};

USTRUCT(BlueprintType)
struct FProjectJLevelConfig : public FTableRowBase
{
	GENERATED_BODY()

	FProjectJLevelConfig()
	{
		
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText LevelName;

	// 禁止编辑
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FProjectJLevelCardPoint> Characters;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FProjectJLevelCardPoint> Landmarks;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FProjectJNavPoint> NavPoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName LuaScriptName;
};
