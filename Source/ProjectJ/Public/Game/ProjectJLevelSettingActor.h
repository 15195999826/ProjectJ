// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectJLevelSettingActor.generated.h"

UCLASS()
class PROJECTJ_API AProjectJLevelSettingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectJLevelSettingActor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Config)
	FVector HandSpellCardStartLocation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Config)
	FVector HandSpellCardOffset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Config)
	FRotator HandSpellCardRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Config)
	FVector TalkWidgetOffset;
};
