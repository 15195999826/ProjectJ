// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/CardType.h"
#include "UObject/Interface.h"
#include "ProjectJCardInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UProjectJCardInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTJ_API IProjectJCardInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	EProjectJCardType GetCardType();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void BindConfig(const FName& InRowName);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FName GetConfigRowName();
};
