#pragma once
#include "CoreMinimal.h"

#include "ProjectJCharacterConfig.generated.h"

USTRUCT(BlueprintType)
struct FProjectJCharacterConfig : public FTableRowBase
{
	GENERATED_BODY()

	FProjectJCharacterConfig(): Health(0), Attack(0), Speed(0)
	{
	}

	UPROPERTY(BlueprintReadOnly)
	FText Name;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName LuaScriptName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="血量"))
	int32 Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="攻击"))
	int32 Attack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="身法"))
	int32 Speed;
};