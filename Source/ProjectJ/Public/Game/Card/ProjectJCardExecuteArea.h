// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/System/ProjectJContextSystem.h"
#include "Game/ProjectJLuaExecutor.h"
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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	// Called every Logic frame
	void CustomTick(int32 InLogicFrameCount);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRoot;
	// 决定边界
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> BoxComponent;

	UFUNCTION(BlueprintNativeEvent)
	void OnFocus();
	UFUNCTION(BlueprintNativeEvent)
	void OnLoseFocus();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowTips(const FText& InTips);

	UFUNCTION(BlueprintImplementableEvent)
	void HideTips();
	
	void StartExecute(AProjectJCardBase* InCard);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsExecuting() const
	{
		return ExecutingCard.IsValid();
	}

	UPROPERTY()
	TWeakObjectPtr<AProjectJCardBase> ExecutingCard;
	UPROPERTY()
	TWeakObjectPtr<AProjectJCardBase> SelectedCard;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void PostExecutePercent(float InPercent);

private:
	EProjectJItemType CachedItemSecondaryType = EProjectJItemType::None;
	int32 CachedMinutes;
	int32 StartFrame;
	bool DuringHiding = false;
	bool StartHidingNextTick = false;
	bool SatisfyFilter(AProjectJCardBase* InCard, const FProjectJTargetFilter& InTargetFilter);

	void OnSelectTarget(AProjectJCardBase* InCard);
};
