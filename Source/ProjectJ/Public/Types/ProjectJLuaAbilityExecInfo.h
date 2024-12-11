#pragma once

#include "CoreMinimal.h"
#include "Item/ProjectJEquipmentConfig.h"
#include "ProjectJLuaAbilityExecInfo.generated.h"


USTRUCT(BlueprintType)
struct FProjectJLuaAbilityAnimation
{
	GENERATED_BODY()
	
	FProjectJLuaAbilityAnimation()
	{
		
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="动画类型"))
	EProjectJAbilityAnimationType AnimationType = EProjectJAbilityAnimationType::Program;

	UPROPERTY()
	int32 TargetID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="资源名字"))
	FString ResourceSoftPath;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="自定义字段(暂时没用)",GetKeyOptions="ProjectJ.ProjectJPropertyHelper.GetAnimCustomKeys"))
	TMap<FName, FString> CustomKV; 
};

USTRUCT(BlueprintType)
struct FProjectJLuaAbilityExecInfo
{
	GENERATED_BODY()

	FProjectJLuaAbilityExecInfo()
	{
		
	}
	
	UPROPERTY()
	TArray<FProjectJLuaAbilityAnimation> Animations;

	// 总时长， Todo: 虽然应该是由动画、特效来决定，但是目前处理难度稍高， 先用配置的总时间来处理
	UPROPERTY()
	float Duration = 1.f;
	
	// 触发功能的时间点
	UPROPERTY()
	float TriggerPoint = 0.5f;
	
	UPROPERTY()
	TArray<int32> TargetIDs;
};
