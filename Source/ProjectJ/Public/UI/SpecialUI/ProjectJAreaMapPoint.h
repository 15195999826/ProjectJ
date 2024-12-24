// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectJAreaMapPoint.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTJ_API UProjectJAreaMapPoint : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Config)
	FText PointText;
};
