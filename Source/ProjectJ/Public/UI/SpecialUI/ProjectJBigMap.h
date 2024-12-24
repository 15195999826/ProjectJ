// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectJBigMap.generated.h"

class UCanvasPanel;
/**
 * 
 */
UCLASS()
class PROJECTJ_API UProjectJBigMap : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCanvasPanel> MapPointRoot;

	UFUNCTION(CallInEditor, BlueprintCallable)
	void BuildMapData();
};
