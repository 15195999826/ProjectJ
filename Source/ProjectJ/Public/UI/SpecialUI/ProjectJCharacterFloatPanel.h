// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "Types/Item/ProjectJItemBase.h"
#include "ProjectJCharacterFloatPanel.generated.h"

class AProjectJCharacter;
class UProjectJAttributeBox;
/**
 * 
 */
UCLASS()
class PROJECTJ_API UProjectJCharacterFloatPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UProjectJAttributeBox> AttackBox;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UProjectJAttributeBox> SpeedBox;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UProjectJAttributeBox> HealthBox;
	
	UFUNCTION(BlueprintImplementableEvent)
	void BindEquipment(const FName& InRowName, EProjectJItemType InType);
	
	void BindCharacter(AProjectJCharacter* InCharacter);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnGetFeature(const FGameplayTag& InFeatureTag);

	UFUNCTION(BlueprintImplementableEvent)
	void OnLoseFeature(const FGameplayTag& InFeatureTag);
	
private:
	void OnGameplayEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle);
	void OnGameplayEffectRemoved(const FActiveGameplayEffect& ActiveGameplayEffect);
};
