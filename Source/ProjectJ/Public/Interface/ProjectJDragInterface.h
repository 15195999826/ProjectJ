// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ProjectJDragInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UProjectJDragInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTJ_API IProjectJDragInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual bool IsDraggable() = 0;
	virtual void OnDragStart() = 0;
	virtual void OnDragTick(float DeltaSeconds, const FVector& ToLocation, float InBaseRotationX,float InMaxShakeDelta, float InLerpSpeed) = 0;
	virtual void OnDrop(float InDropDuration) = 0;
	virtual void OnCancelDrag() = 0;

	virtual FVector GetCurrentLocation() = 0;
};
