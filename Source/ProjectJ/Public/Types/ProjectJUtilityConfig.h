#pragma once

#include "CoreMinimal.h"

#include "ProjectJUtilityConfig.generated.h"

USTRUCT(BlueprintType)
struct FProjectJUtilityConfig : public FTableRowBase
{
	GENERATED_BODY()

	FProjectJUtilityConfig()
	{
		
	}
	
	UPROPERTY(EditAnywhere)
	FText Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName LuaScriptName;
};
