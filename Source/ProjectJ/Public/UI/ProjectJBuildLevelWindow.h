// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectJBuildLevelWindow.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class PROJECTJ_API UProjectJBuildLevelWindow : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(BindWidget))
	TObjectPtr<UTextBlock> EditLevelText;

	UFUNCTION(BlueprintCallable)
	void CreateNewCharacter(const FName& InCharacterName);

	UFUNCTION(BlueprintCallable)
	void CreateNewLandmark(const FName& InLandmarkName);

	UFUNCTION(BlueprintCallable)
	void CreateNewUtility(const FName& InUtilityName);

	UFUNCTION(BlueprintCallable)
	void SaveLevel();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void LoadLevel(FName InLevelRowName);

protected:
	UPROPERTY(BlueprintReadOnly)
	FName EditLevelRowName = NAME_None;
};
