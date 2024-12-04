// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectJEffectActor.generated.h"

class UNiagaraComponent;

UCLASS()
class PROJECTJ_API AProjectJEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectJEffectActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UParticleSystemComponent> CascadeComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category=Config)
	float Duration;

public:
	void StartEffect(bool Reset = true);

	float GetDuration() const { return Duration; }
protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnEffectEnd();

private:
	bool IsRunning = false;
	float Timer = 0.f;
};
