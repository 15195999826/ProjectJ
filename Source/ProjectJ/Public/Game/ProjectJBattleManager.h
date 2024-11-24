// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectJBattleManager.generated.h"

class AProjectJCharacter;
class AProjectJDebugBattleGameMode;

UCLASS()
class PROJECTJ_API AProjectJBattleManager : public AActor
{
	GENERATED_BODY()

	friend AProjectJDebugBattleGameMode;
public:	
	// Sets default values for this actor's properties
	AProjectJBattleManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
protected:
	UPROPERTY()
	TArray<TWeakObjectPtr<AProjectJCharacter>> BattleCharacters;
};
