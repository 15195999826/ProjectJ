#pragma once
#include "CoreMinimal.h"
#include "ProjectJItemBase.h"
#include "Types/CardType.h"
#include "ProjectJPropConfig.generated.h"

USTRUCT(BlueprintType)
struct FProjectJPropConfig : public FProjectJItemBase
{
	GENERATED_BODY()

	FProjectJPropConfig()
	{
		ItemType = EProjectJItemType::Prop;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="是否可以在战斗中使用"))
	bool bCanUseInBattle = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(DisplayName="执行功能脚本"))
	FName ExecLuaScriptName;
};
