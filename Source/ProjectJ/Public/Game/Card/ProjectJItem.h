// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/Card/ProjectJCardBase.h"
#include "Interface/ProjectJCardInterface.h"
#include "ProjectJItem.generated.h"

class UWidgetComponent;
/**
 * 
 */
UCLASS()
class PROJECTJ_API AProjectJItem : public AProjectJCardBase, public IProjectJCardInterface
{
	GENERATED_BODY()

public:
	AProjectJItem();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> FloatWidgetComponent;
	
	// IProjectJCard interface
public:
	virtual EProjectJCardType GetCardType_Implementation() override
	{
		return EProjectJCardType::Item;
	}
	virtual void BindConfig_Implementation(const FName& InRowName) override;

	virtual FName GetConfigRowName_Implementation() override;
};
