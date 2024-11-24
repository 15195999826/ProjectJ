// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "LCCDeveloperSettings.generated.h"

struct FLCCSpecieMenuOption;
class ULCCSpecieConfig;
/**
 * 
 */
UCLASS(config=Game, DefaultConfig, MinimalAPI)
class ULCCDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	virtual FName GetCategoryName() const override;

	// 1、 创建角色菜单的能选择的物种选项
	// 2、 各个物种的配置数据
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "LomoCharacterCreator")
	TArray<FLCCSpecieMenuOption> SpecieConfigs;

	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "LomoCharacterCreator")
	TMap<FGameplayTag, TSoftObjectPtr<UDataTable>> DataTableMap;
};
