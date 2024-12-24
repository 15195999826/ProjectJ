// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayEffectTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UI/ProjectJUIRoot.h"
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
	UFUNCTION(BlueprintCallable, Category="ProjectJ|UI")
	static void EnableUINav();
	UFUNCTION(BlueprintCallable, Category="ProjectJ|UI")
	static void DisableUINav();
	UFUNCTION(BlueprintCallable, Category="ProjectJ|UI")
	static FText GetLocText(const FString& InNameSpace, const FString& InKey, const FText& InDefault);
	UFUNCTION(BlueprintCallable, Category="AutoDW|UI", meta=(WorldContext="WorldContextObject"))
	static void PushUI(const UObject* WorldContextObject,
		EProjectJUIOverlayType OverlayType,
		UUserWidget* Widget,
		TEnumAsByte<EHorizontalAlignment> HorizontalAlignment = HAlign_Fill,
		TEnumAsByte<EVerticalAlignment> VerticalAlignment = VAlign_Fill);

};
