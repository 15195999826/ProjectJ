// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PythonBridge.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTJEDITOR_API UPythonBridge : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = Python)
	static UPythonBridge* Get();

	UFUNCTION(BlueprintImplementableEvent, Category = Python)
	void FunctionImplementedInPython() const;

	UFUNCTION(BlueprintImplementableEvent, Category = Python)
	FString Test() const;

	UFUNCTION(BlueprintImplementableEvent, Category = Python)
	FString ToPinyin(const FString& InText) const;
};
