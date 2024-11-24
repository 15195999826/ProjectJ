// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/ProjectJPlayerControllerBase.h"
#include "ProjectJBuildLevelPC.generated.h"

class IProjectJDragInterface;
class AProjectJCardBase;
/**
 * 
 */
UCLASS()
class PROJECTJ_API AProjectJBuildLevelPC : public AProjectJPlayerControllerBase
{
	GENERATED_BODY()

	
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
	
	
	bool IsDragging = false;
	UPROPERTY()
	TScriptInterface<IProjectJDragInterface> DragActor;

	UFUNCTION(BlueprintImplementableEvent)
	void EditCard(AActor* Actor);

private:
	void OnDragStart(AActor* InDragActor);
	void OnDrop();
	void OnCancelDrag();
};
