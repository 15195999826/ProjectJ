// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "Game/GAS/ProjectJCharacterAttributeSet.h"
#include "ProjectJAttributeBox.generated.h"

class UTextBlock;
class UProgressBar;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class PROJECTJ_API UProjectJAttributeBox : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativePreConstruct() override;
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere,meta=(ExposeOnSpawn))
	FGameplayTag AttributeTag;

	UPROPERTY(BlueprintReadOnly, EditAnywhere,meta=(ExposeOnSpawn))
	FGameplayTag MaxAttributeTag;

	UPROPERTY(BlueprintReadOnly, EditAnywhere,meta=(ExposeOnSpawn))
	bool ShowText = true;

	UPROPERTY(BlueprintReadOnly, EditAnywhere,meta=(ExposeOnSpawn,DisplayName="使用属性减最大作为值"))
	bool UseAttrMinusMaxAsValue = false;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> AttributeName;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> AttributeValue;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> AttributeSpace;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> MaxAttributeValue;

protected:
	TWeakObjectPtr<UAbilitySystemComponent> WeakASCCache;
	
public:
	void Bind(UAbilitySystemComponent* InASC);
	void UnBind();

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void CustomValueText(const FGameplayTag& InTag, float Value, bool& OutCustom, FText& OutText);
	
private:
	bool bHasMaxAttribute = false;
	void OnAttributeChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void OnMaxAttributeChanged(const FOnAttributeChangeData& OnAttributeChangeData);

	void RemoveListeners(const FGameplayAttribute& Attribute);
};
