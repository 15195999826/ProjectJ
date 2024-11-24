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
	TMap<FGameplayTag, FProjectJMontagePostDelegate> MontageEventCallbacks;
	void HandleMontagePostEvent(FGameplayTag EventTag, FProjectJMontageEventData EventData);
};
