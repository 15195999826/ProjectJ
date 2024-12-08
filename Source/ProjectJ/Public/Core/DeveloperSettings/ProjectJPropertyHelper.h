// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ProjectJPropertyHelper.generated.h"

/**
 * 
 */
UCLASS(config=PropertyHelper, DefaultConfig)
class PROJECTJ_API UProjectJPropertyHelper : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category=Cache)
	TMap<FName, FName> CHS2AbilityLuaScriptNameMap;
	
	UFUNCTION()
	static TArray<FName> GetAnimCustomKeys();

	UFUNCTION()
	static TArray<FName> GetLuaAbilityScriptsNames();
};