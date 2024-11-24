// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetSwitcher.h"
#include "LomoWidgetSwitcher.generated.h"

/**
 * 
 */
UCLASS()
class LOMOCHARACTERCREATOR_API ULomoWidgetSwitcher : public UWidgetSwitcher
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category="LCC|Switcher")
	void SetSlotVisibility(int32 Index);
	
	virtual void SetActiveWidgetIndex(int32 Index) override;

#if WITH_EDITOR
	virtual void OnDescendantDeselectedByDesigner(UWidget* DescendantWidget) override;
#endif

	virtual void SynchronizeProperties() override;
};
