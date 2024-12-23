// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Game/Card/ProjectJCardBase.h"
#include "Interface/ProjectJCardInterface.h"
#include "ProjectJUtility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTJ_API AProjectJUtility : public AProjectJCardBase, public IProjectJCardInterface
{
	GENERATED_BODY()

public:
	AProjectJUtility();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> FloatWidgetComponent;
	
	// IProjectJCard interface
public:
	virtual EProjectJCardType GetCardType_Implementation() override
	{
		return EProjectJCardType::Utility;
	}
	virtual void BindConfig_Implementation(const FName& InRowName) override;

	virtual FName GetConfigRowName_Implementation() override;
};
