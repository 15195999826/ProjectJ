// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LomoPCGBtn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLomoPCGBtnClickedSignature, const FString&, StrPayload); 
/**
 * 
 */
UCLASS()
class LOMOCHARACTERCREATOR_API ULomoPCGBtn : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FString StrPayload;
	
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnLomoPCGBtnClickedSignature OnPCGBtnClicked;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void InitPCGBtn(const FString& Label, UTexture2D* Icon, UTexture2D* IconGlow, const FString& InStrPayload);
};
