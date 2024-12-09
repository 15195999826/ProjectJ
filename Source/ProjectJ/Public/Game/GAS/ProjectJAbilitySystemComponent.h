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

DECLARE_MULTICAST_DELEGATE_OneParam(FProjectJMontagePostDelegate, FProjectJMontageEventData*)
/**
 * 
 */
UCLASS()
class PROJECTJ_API UProjectJAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UProjectJAbilitySystemComponent();
	
	TMap<FGameplayTag, FProjectJMontagePostDelegate> MontageEventCallbacks;
	void HandleMontagePostEvent(FGameplayTag EventTag, FProjectJMontageEventData EventData);

private:
	TMap<FGameplayTag, FDelegateHandle> GEStackCountListenerMap;
	void OnGameplayEffectStackChange(FActiveGameplayEffectHandle InGEHandle, int32 NewStackCount, int32 OldStackCount);
	void OnGEAppliedToSelf(UAbilitySystemComponent* SourceASC, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle);
	void OnGERemoved(const FActiveGameplayEffect& ActiveGameplayEffect);
	
};
