// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectJBuildLevelTipsActor.generated.h"

UCLASS()
class PROJECTJ_API AProjectJBuildLevelTipsActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectJBuildLevelTipsActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BuildLevel")
	TObjectPtr<USceneComponent> SceneRoot;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BuildLevel")
	TObjectPtr<class UTextRenderComponent> TextRenderComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Todo: 更新Tips内容

};
