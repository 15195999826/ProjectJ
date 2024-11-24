// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Card/ProjectJCardBase.h"
#include "GameFramework/Actor.h"
#include "ProjectJSceneUIManager.generated.h"

class UWidgetComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FProjectJOnTalkOver);

UCLASS()
class PROJECTJ_API AProjectJSceneUIManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectJSceneUIManager();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRoot;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> TalkWidgetComponent;


	UPROPERTY(BlueprintCallable)
	FProjectJOnTalkOver OnTalkOver;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent)
	void StartTalkWith(AProjectJCardBase* InCard);
};
