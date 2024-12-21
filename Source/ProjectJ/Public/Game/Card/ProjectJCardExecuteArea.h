// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectJCardExecuteArea.generated.h"

class AProjectJCardBase;
class UBoxComponent;
/**
 * 
 */
UCLASS()
class PROJECTJ_API AProjectJCardExecuteArea : public AActor
{
	GENERATED_BODY()

public:
	AProjectJCardExecuteArea();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRoot;
	// 决定边界
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> BoxComponent;

	UFUNCTION(BlueprintNativeEvent)
	void OnFocus();
	UFUNCTION(BlueprintNativeEvent)
	void OnLoseFocus();
	
	void StartExecute(AProjectJCardBase* InCard);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsExecuting() const
	{
		return ExecutingCard.IsValid();
	}

	UPROPERTY()
	TWeakObjectPtr<AProjectJCardBase> ExecutingCard;
};
