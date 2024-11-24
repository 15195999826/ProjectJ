// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LomoGroupButton.h"
#include "Blueprint/UserWidget.h"
#include "LomoButtonGroup.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeActiveButton, int32, InIntPayload);
/**
 * 按钮组，用于管理一组按钮， 内部的Button必须是ULomoGroupButton的子类
 */
UCLASS()
class LOMOCHARACTERCREATOR_API ULomoButtonGroup : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnChangeActiveButton OnChangeActiveButton;

protected:
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<UPanelWidget> ButtonContainer;

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<ULomoGroupButton> ActiveButton;
public:
	UFUNCTION(BlueprintCallable)
	void ClearGroup();
	
	UFUNCTION(BlueprintCallable)
	void PcgButtonGroup(TSubclassOf<ULomoGroupButton> BtnClass, TArray<FLomoGroupBtnConfig> InMenuConfig);

	UFUNCTION(BlueprintCallable)
	ULomoGroupButton* AppendPcgButton(TSubclassOf<ULomoGroupButton> BtnClass, FLomoGroupBtnConfig InBtnConfig);
	
	UFUNCTION(BlueprintCallable)
	void DeActiveAll();

	UFUNCTION(BlueprintCallable)
	void SetDefaultActiveButton(int32 InIndex);

protected:
	UFUNCTION(BlueprintCallable)
	void BtnGroupNativePreConstruct(UPanelWidget* InButtonContainer);
	
	UFUNCTION(BlueprintImplementableEvent)
	void CustomPcgBtnStyle(ULomoGroupButton* InBtn);
	
	void OnButtonActive(ULomoGroupButton* InGroupButton, int32 InIntPayload);
};



