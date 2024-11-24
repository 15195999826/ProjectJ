// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ProjectJGameInstance.generated.h"

class UProjectJUIRoot;
/**
 * 
 */
UCLASS()
class PROJECTJ_API UProjectJGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UProjectJUIRoot> UIRoot;

	UFUNCTION(BlueprintImplementableEvent)
	void ProtectInitUIRoot();
};
