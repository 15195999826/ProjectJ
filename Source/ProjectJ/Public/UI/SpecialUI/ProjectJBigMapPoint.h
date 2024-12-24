// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectJBigMapPoint.generated.h"

class UProjectJAreaMap;
/**
 * 
 */
UCLASS()
class PROJECTJ_API UProjectJBigMapPoint : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Config)
	FText PointText;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Config)
	TSubclassOf<UProjectJAreaMap> AreaMapClass;
};
