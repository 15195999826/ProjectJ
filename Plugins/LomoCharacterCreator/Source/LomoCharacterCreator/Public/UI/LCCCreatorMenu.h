// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LCCSpecieConfig.h"
#include "Blueprint/UserWidget.h"
#include "LCCCreatorMenu.generated.h"

class UHorizontalBox;
class ULCCSpecieConfig;
/**
 * 
 */
UCLASS()
class LOMOCHARACTERCREATOR_API ULCCCreatorMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	UFUNCTION(BlueprintCallable)
	void NativeInit(
		UHorizontalBox* InSpecieBox
		);


	UFUNCTION(BlueprintImplementableEvent)
	void OnPCGSpecieBoxAddNewBtn(ULomoPCGBtn* NewBtn, const FString& String, UTexture2D* Texture2D, UTexture2D* LoadSynchronous, const TSoftObjectPtr<ULCCSpecieConfig>& SpecieConfigSoft);
};
