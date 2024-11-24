// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/ProjectJPlayerControllerBase.h"
#include "Types/ProjectJGameStage.h"
#include "ProjectJPlayerController.generated.h"

class AProjectJCardBase;
class IProjectJDragInterface;
/**
 * 
 */
UCLASS()
class PROJECTJ_API AProjectJPlayerController : public AProjectJPlayerControllerBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
protected:
	virtual void CustomTick(float DeltaSeconds, bool OverWidget, bool IsHitGround, const FVector& HitGroundLocation, AActor* InHitActor) override;

protected:
	UPROPERTY(EditAnywhere, Category = "DragCard")
	float ProjectileDistance = 400.f;
	UPROPERTY(EditAnywhere, Category = "DragCard")
	float MaxShakeDelta = 15.f;
	UPROPERTY(EditAnywhere, Category = "DragCard")
	float BaseRotationX = 0.f;
	UPROPERTY(EditAnywhere, Category = "DragCard")
	float LerpSpeed = 15.f;
	UPROPERTY(EditAnywhere, Category = "DragCard")
	float DropOnGroundDuration = 0.2f;
	

	bool IsPending = false;
	
	bool IsDragging = false;
	UPROPERTY()
	TScriptInterface<IProjectJDragInterface> DragActor;

private:
	void OnDragStart(AActor* InDragActor);
	void OnDrop();
	void OnCancelDrag();

	void OnStageChange(EProjectJGameStage OldStage, EProjectJGameStage NewStage);
	
};
