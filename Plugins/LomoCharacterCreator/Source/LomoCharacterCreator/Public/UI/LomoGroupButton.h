// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LCCTypes.h"
#include "Blueprint/UserWidget.h"
#include "LomoGroupButton.generated.h"


/*
 * Struct : UI Item
 */
/**
 * 放在ButtonGroup中的Button的基类
 */
UCLASS()
class LOMOCHARACTERCREATOR_API ULomoGroupButton : public UUserWidget
{
	GENERATED_BODY()
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGroupButtonClicked, ULomoGroupButton*, int32);
public:
	// 选中按钮
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Active();

	// 取消选中
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DeActive();

	// 设置配置
	UFUNCTION(BlueprintCallable)
	void SetConfig(FLomoGroupBtnConfig InConfig);

	FOnGroupButtonClicked OnGroupButtonActive;
	
protected:
	// 按钮的配置
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FLomoGroupBtnConfig Config;

	// 选中状态
	UPROPERTY(BlueprintReadWrite)
	bool bActive = false;
};
