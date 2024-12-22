// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Types/ProjectJLuaInstanceType.h"
#include "ProjectJEditorBFL.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTJEDITOR_API UProjectJEditorBFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static bool CreateLuaScript(const FName& InRowName, const FString& InLuaScriptName, EProjectJLuaInstanceType InType, bool ReportErrorDialog = true);	
};
