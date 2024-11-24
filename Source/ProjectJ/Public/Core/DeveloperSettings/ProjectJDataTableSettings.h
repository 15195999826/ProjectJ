// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ProjectJDataTableSettings.generated.h"

/**
 * 
 */
UCLASS(config=Game, DefaultConfig, MinimalAPI)
class UProjectJDataTableSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	virtual FName GetCategoryName() const override;
	
	UPROPERTY(config, EditAnywhere, Category = DataTable)
	TSoftObjectPtr<UDataTable> CharacterTable;

	UPROPERTY(config, EditAnywhere, Category = DataTable)
	TSoftObjectPtr<UDataTable> LandmarkTable;

	UPROPERTY(config, EditAnywhere, Category = DataTable)
	TSoftObjectPtr<UDataTable> LevelTable;
	
	UPROPERTY(config, EditAnywhere, Category = DataTable)
	TSoftObjectPtr<UDataTable> AbilityTable;

	UPROPERTY(config, EditAnywhere, Category = DataTable)
	TSoftObjectPtr<UDataTable> WeaponTable;
	UPROPERTY(config, EditAnywhere, Category = DataTable)
	TSoftObjectPtr<UDataTable> ArmorTable;
};
