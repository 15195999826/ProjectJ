// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LomoModalWidget.generated.h"

enum class ELomoModalCloseResult : uint8;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLomoModalEventSignature, ULomoModalWidget*, UserWidget, ELomoModalCloseResult, Result);

/**
 * 
 */
UCLASS()
class LOMOCHARACTERCREATOR_API ULomoModalWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PerformClose(ELomoModalCloseResult Result);

	UFUNCTION(BlueprintImplementableEvent)
	void Setup(const FText& InMessage);
	
	/* Delegate part */
	UPROPERTY(BlueprintAssignable, Category = "LCC|EventDispatchers")
	FLomoModalEventSignature OnClose;
};
