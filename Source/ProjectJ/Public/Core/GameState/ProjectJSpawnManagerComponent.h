// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectJSpawnManagerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTJ_API UProjectJSpawnManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProjectJSpawnManagerComponent();
	virtual void InitializeComponent() override;

	UFUNCTION(BlueprintCallable)
	bool TryFindPlayerStart(const FName& Tag, AActor*& OutStart);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void OnLevelAdded(ULevel* InLevel, UWorld* InWorld);
	void HandleOnActorSpawned(AActor* SpawnedActor);

private:
	// AActor* ChoosePlayerStart(AController* Player);
	friend class AProjectJGameModeBase; // Friend class to allow the game mode to call the ChoosePlayerStart function
	
	// UPROPERTY(Transient)
	// TArray<TWeakObjectPtr<APlayerStart>> CachedPlayerStarts;

		
};
