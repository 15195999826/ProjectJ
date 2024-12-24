#pragma once
#include "CoreMinimal.h"

#include "ProjectJMapPoint.generated.h"

USTRUCT(BlueprintType)
struct FProjectJMapPoint
{
	GENERATED_BODY()
	
	FProjectJMapPoint(){}

	// 该点的名称
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName Name;

	// 该点之下的所有区域点
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FName> AreaPointNames; 
};
