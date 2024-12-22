// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ProjectJEditorDebugBFL.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTJ_API UProjectJEditorDebugBFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void DebugPauseEditor();
};
