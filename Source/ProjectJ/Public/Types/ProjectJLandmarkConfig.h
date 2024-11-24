#pragma once
#include "CoreMinimal.h"

#include "ProjectJLandmarkConfig.generated.h"

USTRUCT(BlueprintType)
struct FProjectJLandmarkConfig : public FTableRowBase
{
	GENERATED_BODY()

	FProjectJLandmarkConfig()
	{
		
	}
	
	UPROPERTY(EditAnywhere)
	FText Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName LuaScriptName;
};
