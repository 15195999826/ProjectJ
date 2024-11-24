// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LomoModalWidget.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "LomoAsyncModalTask.generated.h"

/**
 * 
 */
UCLASS()
class LOMOCHARACTERCREATOR_API ULomoAsyncModalTask : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	UPROPERTY(BlueprintAssignable)
	FLomoModalEventSignature OnShow;
	
	UPROPERTY(BlueprintAssignable)
	FLomoModalEventSignature OnClose;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"),
	Category = "LCC|UI")
	static ULomoAsyncModalTask* AsyncShowModalFromClass(const UObject* WorldContextObject, TSubclassOf<ULomoModalWidget> Class, FText InMessage);
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"),
		Category = "LCC|UI")
	static ULomoAsyncModalTask* AsyncShowModalFromRef(const UObject* WorldContextObject, ULomoModalWidget* ModalBaseWidget);

	UFUNCTION()
	void NativeOnClose(ULomoModalWidget* UserWidget, ELomoModalCloseResult Result);
	
	// UBlueprintAsyncActionBase interface
	virtual void Activate() override;
	//~UBlueprintAsyncActionBase interface

	// Allows the Object to use BP_Functions
	virtual class UWorld* GetWorld() const override;
	
private:
	const UObject* WorldContextObject;
	
	UPROPERTY()
	ULomoModalWidget* ModalRef = nullptr;
};
