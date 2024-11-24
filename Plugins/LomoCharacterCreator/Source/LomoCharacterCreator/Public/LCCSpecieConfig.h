// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "LCCTypes.h"
#include "Engine/DataAsset.h"
#include "LCCSpecieConfig.generated.h"

class ULomoPCGBtn;

USTRUCT(BlueprintType)
struct FLCCSpecieTuple
{
	GENERATED_BODY()

	FLCCSpecieTuple():
	Maturity(ELCCMaturity::None), Gender(ELCCGender::None)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ELCCMaturity Maturity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ELCCGender Gender;
};

USTRUCT(BlueprintType)
struct FLCCSectionContentParams
{
	GENERATED_BODY()

	FLCCSectionContentParams()
	{
	}

	// 从LCCDeveloperSettings中读取Tag对应的DataTable
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FGameplayTag> DataTableTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FGameplayTag, bool> BoolParams;

	// 先检查Bool是否满足，再检查DisableCondition是否禁用该SectionContent
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FGameplayTag, FLCCSpecieTuple> DisableConditions;
	
	// 自定义参数
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FString> CustomParams;
};

// 可以配置哪些Category（身体、头发、眼睛等）
// Category中，有哪些Section（如，身体可以配置体型、身高）， 各个Section对应哪个Panel， 这个SectionContent接收的Config（比如，Panel中哪些内容需要关闭，这些内容使用哪个DataTable）
USTRUCT(BlueprintType)
struct FLCCSectionConfig
{
	GENERATED_BODY()
	
	FLCCSectionConfig():
	SectionContentClass(nullptr)
	{
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLomoGroupBtnConfig SectionBtnConfig;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UUserWidget> SectionContentClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLCCSectionContentParams SectionContentParams;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FLCCSpecieTuple> DisableCondition;
};

USTRUCT(BlueprintType)
struct FLCCCategoryConfig
{
	GENERATED_BODY()
	
	FLCCCategoryConfig(){}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLomoGroupBtnConfig CategoryBtnConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FLCCSectionConfig> Sections;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FLCCSpecieTuple> DisableCondition;
};

/**
 * 
 */
UCLASS(BlueprintType)
class LOMOCHARACTERCREATOR_API ULCCSpecieConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	// 可以选择哪些年龄段
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<ELCCMaturity> Maturities;
	// 可以选择哪些性别
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<ELCCGender> Genders;
	// Todo:根据性别和年龄段， 区分可以有哪些菜单

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FLCCCategoryConfig> Categories;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsSpecieTupleDisable(ELCCMaturity InMaturity, ELCCGender InGender, const TArray<FLCCSpecieTuple>& SpecieTuple);
};

USTRUCT(BlueprintType)
struct FLCCSpecieMenuOption
{
	GENERATED_BODY()
	FLCCSpecieMenuOption(): MenuOrder(0)
	{
	};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MenuOrder;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ULomoPCGBtn> SpecieButtonClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString SpecieBtnLabel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> BtnIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> BtnIconGlow;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<ULCCSpecieConfig> SpecieConfig;
};
