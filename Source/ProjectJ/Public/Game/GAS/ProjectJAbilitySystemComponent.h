// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "ProjectJAbilitySystemComponent.generated.h"

USTRUCT(BlueprintType)
struct FProjectJMontageEventData
{
	GENERATED_BODY()
	
	FProjectJMontageEventData(): IntPayload(0)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayAbilityTriggerPayload)
	int32 IntPayload;
};

/**
 * 由于不存在一次性Apply多层GE的函数，每次GEStack Count 增加时， 数值只能是1
 * GE每一层的数据， 在哪回合获得， 持续时间多久 
 */
USTRUCT(BlueprintType)
struct FProjectJGELayerData
{
	GENERATED_BODY()
	
	FProjectJGELayerData()
	{
	}

	UPROPERTY()
	int32 LayerID = 0;

	UPROPERTY()
	int32 GetRound = 0;

	UPROPERTY()
	int32 Duration = 0;
};

// 运行时持久化的数据， 跟SpecHandle一一对应
USTRUCT(BlueprintType)
struct FProjectJCustomStackedGESpec
{
	GENERATED_BODY()
	
	FProjectJCustomStackedGESpec()
	{
	}
	
	FDelegateHandle StackCountChangedHandle;

	int32 GLayerID = 0;
	
	/**
	 * 只记录每一轮的StackCount的增加
	 */
	UPROPERTY()
	TArray<FProjectJGELayerData> EachLayerData;
	
	UPROPERTY()
	TMap<int32, int32> StackCountReduceAtRoundMap;
};
DECLARE_MULTICAST_DELEGATE_OneParam(FProjectJMontagePostDelegate, FProjectJMontageEventData*)
/**
 * 
 */
UCLASS()
class PROJECTJ_API UProjectJAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

	inline static FProjectJCustomStackedGESpec EmptyCustomStackedGESpec = FProjectJCustomStackedGESpec();
	
public:
	UProjectJAbilitySystemComponent();
	
	TMap<FGameplayTag, FProjectJMontagePostDelegate> MontageEventCallbacks;
	void HandleMontagePostEvent(FGameplayTag EventTag, FProjectJMontageEventData EventData);

	UFUNCTION(BlueprintCallable, Category = GameplayEffects)
	bool CustomRemoveActiveGameplayEffect(const FGameplayTag& InFeatureTag, const TArray<int32> LayerIDs);

	const FProjectJCustomStackedGESpec* GetCustomStackedGESpec(const FGameplayTag& InFeatureTag);
	int32 GetStackCount(const FGameplayTag& FeatureTag);

private:
	UPROPERTY()
	TMap<FActiveGameplayEffectHandle, FProjectJCustomStackedGESpec> CustomStackedGESpecMap;
	UPROPERTY()
	TMap<FActiveGameplayEffectHandle, FGameplayTag> Handle2FeatureTagMap;

	void OnGameplayEffectStackChange(FActiveGameplayEffectHandle InGEHandle, int32 NewStackCount, int32 OldStackCount);
	void OnGEAppliedToSelf(UAbilitySystemComponent* SourceASC, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle);
	void OnGERemoved(const FActiveGameplayEffect& ActiveGameplayEffect);
	
};
