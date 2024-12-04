// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayEffectTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ProjectJBlueprintFunctionLibrary.generated.h"

class UGameplayEffect;
/**
 * 
 */
UCLASS()
class PROJECTJ_API UProjectJBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="TheOne|UI")
	static void EnableUINav();
	UFUNCTION(BlueprintCallable, Category="TheOne|UI")
	static void DisableUINav();
	UFUNCTION(BlueprintCallable, Category="TheOne|UI")
	static FText GetLocText(const FString& InNameSpace, const FString& InKey, const FText& InDefault);
};
