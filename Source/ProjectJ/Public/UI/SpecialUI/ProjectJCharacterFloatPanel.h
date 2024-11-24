// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/Item/ProjectJItemBase.h"
#include "ProjectJCharacterFloatPanel.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTJ_API UProjectJCharacterFloatPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void BindEquipment(const FName& InRowName, EProjectJItemType InType);
};
