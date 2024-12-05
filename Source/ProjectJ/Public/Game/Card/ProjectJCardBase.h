// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildLevel/ProjectJBuildLevelPC.h"
#include "GameFramework/Actor.h"
#include "Interface/ProjectJDragInterface.h"
#include "Types/CardType.h"
#include "ProjectJCardBase.generated.h"

class UTextRenderComponent;
class UPaperSpriteComponent;
class UProjectJContextSystem;
class UBoxComponent;

UCLASS(Abstract)
class PROJECTJ_API AProjectJCardBase : public AActor, public IProjectJDragInterface
{
	GENERATED_BODY()

	friend UProjectJContextSystem;
public:	
	// Sets default values for this actor's properties
	AProjectJCardBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> Collider;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPaperSpriteComponent> FrameSprite;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UTextRenderComponent> Title;

	int32 ID;
	bool CanDrag = true;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY()
	FName ConfigRowName;
	
	FVector DragStartLocation;

public:
	// Start IProjectJDragInterface
	virtual bool IsDraggable() override
	{
		return CanDrag;
	}
	virtual void OnDragStart() override;
	virtual void OnDragTick(float DeltaSeconds, const FVector& ToLocation, float InBaseRotationX,float InMaxShakeDelta, float InLerpSpeed) override;
	virtual void OnDrop(float InDuration) override;
	virtual void OnCancelDrag() override;
	// End IProjectJDragInterface


	void OnSpellFocus();
	void OnLoseSpellFocus();
	
	////// 程序动画 ////
private:
	

	// 1. 拖拽卡牌
	FTimerHandle DropOnGroundTimerHandle;
	float DropOnGroundStartTime;
	float DropOnGroundDuration = 0.5f;
	FVector DropOnGroundStartLocation;
	void UpdateDropOnGroundAnimation();
	
};
