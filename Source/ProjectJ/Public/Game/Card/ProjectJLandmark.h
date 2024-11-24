// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/Card/ProjectJCardBase.h"
#include "Interface/ProjectJCardInterface.h"
#include "ProjectJLandmark.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTJ_API AProjectJLandmark : public AProjectJCardBase, public IProjectJCardInterface
{
	GENERATED_BODY()

	// IProjectJCard interface
public:
	virtual EProjectJCardType GetCardType_Implementation() override
	{
		return EProjectJCardType::Landmark;
	}
	virtual void BindConfig_Implementation(const FName& InRowName) override;

	virtual FName GetConfigRowName_Implementation() override;
	
};
