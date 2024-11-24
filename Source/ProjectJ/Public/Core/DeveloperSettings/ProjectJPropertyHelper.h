// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ProjectJPropertyHelper.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTJ_API UProjectJPropertyHelper : public UDeveloperSettings
{
	GENERATED_BODY()


public:
	UFUNCTION()
	static TArray<FName> GetStarOneCharacters();
	UFUNCTION()
	static TArray<FName> GetStarTwoCharacters();
	UFUNCTION()
	static TArray<FName> GetStarThreeCharacters();
	UFUNCTION()
	static TArray<FName> GetStarFourCharacters();
	UFUNCTION()
	static TArray<FName> GetStarFiveCharacters();
	UFUNCTION()
	static TArray<FName> GetStarSixCharacters();
	UFUNCTION()
	static TArray<FName> GetAbilityRowNames();
};
