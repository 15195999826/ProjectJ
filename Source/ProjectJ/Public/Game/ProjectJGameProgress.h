// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Card/ProjectJCardBase.h"
#include "GameFramework/Actor.h"
#include "Types/ProjectJGameStage.h"
#include "Types/ProjectJLevelConfig.h"
#include "ProjectJGameProgress.generated.h"

USTRUCT()
struct FProjectJChangeStagePayload
{
	GENERATED_BODY()

	FProjectJChangeStagePayload(){}

	TWeakObjectPtr<AProjectJCardBase> CardPayload;
};

UCLASS()
class PROJECTJ_API AProjectJGameProgress : public AActor
{
	GENERATED_BODY()

	inline static FProjectJChangeStagePayload EmptyPayload = FProjectJChangeStagePayload();
	
public:	
	// Sets default values for this actor's properties
	AProjectJGameProgress();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	float DeltaTimeAccumulator = 0.f;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FDataTableRowHandle StartLevel;

	// Runtime
	UPROPERTY(BlueprintReadOnly)
	EProjectJGameStage GameStage = EProjectJGameStage::Idle;

	inline static FName SignalObserving = TEXT("SignalObserving");
	inline static FName SignalPerforming = TEXT("SignalPerforming");
	UPROPERTY()
	TMap<FName, bool> WaitingSignals;
	
public:
	UFUNCTION(BlueprintCallable)
	void StartNewGame();
	
	void EnterLevel(const FName& LevelRowName);
	
private:
	void OnLevelPrepared();
	void SpellCardToArea(AProjectJCardBase* InCard, int32 Index, const FVector& CenterLocation, const FVector& Offset);

	void OnLeaveStage(EProjectJGameStage OldStage, FProjectJChangeStagePayload Payload);
	void ChangeStage(EProjectJGameStage NewStage, const FProjectJChangeStagePayload& Payload);

	
	void OnEnterStage(const FProjectJChangeStagePayload& Payload);

	void OnPerformEnd();
	
	void OnObserveCard(AProjectJCardBase* InCard);
	void OnCheckCard(AProjectJCardBase* InCard);
	void OnHideAtCard(AProjectJCardBase* InCard);
	void OnAmbushCard(AProjectJCardBase* InCard);

protected:
	UFUNCTION()
	void OnTalkOver();
};
