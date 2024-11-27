// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectJDebugBattleGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTJ_API AProjectJDebugBattleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:

	
public:
	UFUNCTION(BlueprintCallable)
	void RegisterTeamCharacter(int32 TeamID, const FString& InPosition, const FString& InCharacterRow, const FString& InWeaponRow, const FString& InArmorRow);

	UFUNCTION(BlueprintCallable)
	void EnterDebugBattle();

private:
	
};
