// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ProjectJGeneralSettings.generated.h"

class UProjectJAttackGA;
class AProjectJNavPointActor;
class AProjectJSpell;
class AProjectJLandmark;
class AProjectJCharacter;
class AProjectJGameProgress;
class UGameplayEffect;
/**
 * 
 */
UCLASS(config=Game, DefaultConfig, MinimalAPI)
class UProjectJGeneralSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	virtual FName GetCategoryName() const override;

	UPROPERTY(config, EditAnywhere, Category=GAS)
	TSubclassOf<UGameplayEffect> InitAttributeEffect;

	UPROPERTY(config, EditAnywhere, Category=GAS)
	TSubclassOf<UGameplayEffect> EquipmentAttributeEffect;
	
	UPROPERTY(config, EditAnywhere, Category=GAS)
	TSubclassOf<UProjectJAttackGA> AttackGAClass;

	// Todo: 技能通用的一个技能

	UPROPERTY(config, EditAnywhere)
	TSubclassOf<AProjectJGameProgress> GameProgressClass;

	UPROPERTY(config, EditAnywhere)
	TSubclassOf<AProjectJCharacter>  CharacterClass;

	UPROPERTY(config, EditAnywhere)
	TSubclassOf<AProjectJLandmark> LandmarkClass;

	UPROPERTY(config, EditAnywhere)
	TSubclassOf<AProjectJSpell> SpellClass;
	
	UPROPERTY(config, EditAnywhere)
	TSubclassOf<AProjectJNavPointActor> NavPointClass;
	
};