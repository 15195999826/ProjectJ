// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectJBuildLevelGameMode.generated.h"

class AProjectJBuildLevelTipsActor;
/**
 * 
 */
UCLASS()
class PROJECTJ_API AProjectJBuildLevelGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void StartPlay() override;
	
	FSimpleMulticastDelegate OnLevelPrepared;

	UPROPERTY(EditDefaultsOnly, Category = "BuildLevel")
	TSubclassOf<AProjectJBuildLevelTipsActor> TipsActorClass;

	UFUNCTION(BlueprintCallable)
	void AttachTips(AActor* InActor, FVector TipsOffset);

	UFUNCTION(BlueprintCallable)
	void DetachTips(AActor* InActor);
	
private:
	UPROPERTY()
	TMap<uint32, TObjectPtr<AProjectJBuildLevelTipsActor>> TipsActors;
};
