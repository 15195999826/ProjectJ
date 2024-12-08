// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectUIData.h"
#include "ProjectJGameplayEffectUIData.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTJ_API UProjectJGameplayEffectUIData : public UGameplayEffectUIData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Data)
	TSoftObjectPtr<UTexture2D> Icon;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Data)
	FText Title;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Data, meta = (MultiLine = "true"))
	FText Description;
};
