// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectJBigMap.generated.h"

class UProjectJBigMapPoint;
class UCanvasPanel;

/**
 * 传递RowName和用于UI显示的名称
 */
USTRUCT(BlueprintType)
struct FProjectJDungeonSimpleInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FName RowName;

	UPROPERTY(BlueprintReadOnly)
	FText DisplayText;
};
/**
 * 
 */
UCLASS()
class PROJECTJ_API UProjectJBigMap : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCanvasPanel> MapPointRoot;

	UFUNCTION(CallInEditor, BlueprintCallable)
	void BuildMapData();

protected:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FProjectJDungeonSimpleInfo> GetDungeonsAtAreaPoint(const FName& AreaPointName) const;
	
private:
	// MapPoint缓存
	UPROPERTY()
	TMap<FName, TObjectPtr<UProjectJBigMapPoint>> MapPointCache;
	
	void OnDungeonsRefresh();
};
