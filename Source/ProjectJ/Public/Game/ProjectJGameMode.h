// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectJGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTJ_API AProjectJGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void StartPlay() override;

public:
	FSimpleMulticastDelegate OnLevelPrepared;
};
