// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpringArmCameraActor.h"
#include "GameFramework/PlayerController.h"
#include "ProjectJPlayerControllerBase.generated.h"
UENUM()
enum class EProjectJMouseState : uint8
{
	Invalid = 0,
	Idle = 1,
	Press = 2,
	Pressing = 3,
	Release = 4
};

/**
 * 
 */
UCLASS()
class PROJECTJ_API AProjectJPlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<ASpringArmCameraActor> CurrentCamera;

	UPROPERTY()
	EProjectJMouseState LeftMouseState = EProjectJMouseState::Idle;

	UPROPERTY()
	EProjectJMouseState RightMouseState = EProjectJMouseState::Idle;

	virtual void CustomTick(float DeltaSeconds, bool OverWidget, bool IsHitGround, const FVector& HitGroundLocation, AActor* InHitActor) {};

private:
	void SampleMouseState(EProjectJMouseState& MouseState, bool IsJustPressed, bool IsDown);
};
