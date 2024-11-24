// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/ProjectJDragInterface.h"
#include "Types/ProjectJLevelConfig.h"
#include "ProjectJNavPointActor.generated.h"

class UTextRenderComponent;
class UBoxComponent;

UCLASS()
class PROJECTJ_API AProjectJNavPointActor : public AActor, public IProjectJDragInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectJNavPointActor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ToLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanDrag = false;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UTextRenderComponent> Title;

	UPROPERTY()
	int32 ID;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Start IProjectJDragInterface
public:
	virtual bool IsDraggable() override
	{
		return CanDrag;
	}
	virtual void OnDragStart() override;
	virtual void OnDragTick(float DeltaSeconds, const FVector& ToLocation, float InBaseRotationX,float InMaxShakeDelta, float InLerpSpeed) override;
	virtual void OnDrop(float InDropDuration) override;
	virtual void OnCancelDrag() override;

private:
	FVector DragStartLocation;
	// End IProjectJDragInterface
public:
	void BindConfig(const FProjectJNavPoint& InConfig);

protected:
	UFUNCTION()
	void OnNavPointClicked(AActor* TouchedActor, FKey ButtonPressed);
	
};
